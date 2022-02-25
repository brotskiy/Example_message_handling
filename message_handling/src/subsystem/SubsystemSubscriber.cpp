
#include <QDateTime>
#include <QDebug>

#include <algorithm>
#include <iterator>

#include "helper/miscellanea.h"
#include "wisdom/calc.h"

Q_DECLARE_METATYPE(uint32_t)
Q_DECLARE_METATYPE(subscriber::Settings)

namespace
{
    const HandlerThread PROTOCOL_HANDLER_THREAD = CHR_THREAD;
}

// :::::::::::::::::::::::::::::::::::::::: Содержимое. ::::::::::::::::::::::::::::::::::::::::

subscriber::__Settings::__Settings(const subscriber::Settings& postSttngs) :
    subscriberId_(subscriberSttngs.subscriberId_),
    groupId_(subscriberSttngs.groupId_),
    subscriberName_(subscriberSttngs.subscriberName_),
    netProtocolLogPeriod_ms_(subscriberSttngs.netProtocolLogPeriod_ms_),
    position_(subscriberSttngs.position_),
    isConnected_(false)
{
    position_.timeData_ms_ = QDateTime::currentMSecsSinceEpoch();
}

// :::::::::::::::::::::::::::::::::::::::: Создание. ::::::::::::::::::::::::::::::::::::::::

subscriber::subscriber(const subscriber::Settings& subscriberSttngs,
                           CommonNetProtocolSupport* p_commonProtocol,
                           PARNetProtocolSupport* p_parProtocol,
                           QObject* parent) :
    QObject(parent),
    settings_(subscriberSttngs),
    p_protoCommon_(p_commonProtocol)
{
    createWorkData();
    registerTypes();
    createsubscriberBase();
    createConnections();
}

void subscriber::createWorkData()
{

    using SOME_NAMESPACE::PresetID_struct;
    using SOME_NAMESPACE::Preset_struct;

    data_.p_presets_.reset( new PARDataContainer<PresetID_struct, Preset_struct>(create_PARPresetIdExtractor()) );
}

void subscriber::registerTypes()
{
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<subscriber::Settings>();
}

void subscriber::createsubscriberBase()
{
    createsubscriberBaseCommon();
    createsubscriberBasePAR();
}

void subscriber::createsubscriberBaseCommon()
{
    tech::subscriberBaseCommon::Parameters params;

    params.callbacks_.getsubscriberId          = create_getsubscriberId();
    params.callbacks_.anysubscriberIsAllowed   = create_anysubscriberIsAllowed();
    params.callbacks_.getCourse          = create_getCourse();
    params.callbacks_.checkProtoVersion  = create_checkProtoVersion();
    params.callbacks_.getTime_ms         = create_getTime_ms();
    params.callbacks_.setTime_ms         = create_setTime_ms();

    p_subscriberCommon_ = new tech::subscriberBaseCommon(params, this);    // за уничтожение отвечает объект-родитель!
}

void subscriber::createConnections()
{
    using tech::subscriberBaseCommon;

    connect(p_protoCommon_, &CommonNetProtocolSupport::logMessage, this, &subscriber::netLogMessage);

    subscriberBaseCommon*const subscriberCommon = getsubscriberBaseCommon();

    connect(p_protoCommon_, &CommonNetProtocolSupport::connectionStatus,
            this,           &subscriber::onConnectionStatus);

    tech::helper::connectNetProtocolCommon(p_protoCommon_, subscriberCommon);

    connect(subscriberCommon, &subscriberBaseCommon::internal_common_SetsubscriberUseStatePack, this,
    [this](std::shared_ptr<std::vector<SOME_NAMESPACE::groups::Usesubscriber_struct>> p_states) -> void
    {
        data_.usesubscriber_ = p_states->front();    // вектор - это просто обертка над ЕДИНСТВЕННЫМ элементом!
    });
}

CommonNetProtocolSupport* subscriber::createCommonNetProtocolSupport(const subscriber::Settings& subscriberSttngs,
                                                                       NET_NAMESPACE::NET_CLASS* p_netWorker,
                                                                       QObject* parent)
{
    CommonNetProtocolSupport::Settings protoSttngs;

    protoSttngs.partnerNetName_ = subscriberSttngs.subscriberName_;
    protoSttngs.workThread_     = PROTOCOL_HANDLER_THREAD;
    protoSttngs.logPeriod_ms_   = subscriberSttngs.netProtocolLogPeriod_ms_;

    return new CommonNetProtocolSupport(protoSttngs, p_netWorker, parent);
}

void subscriber::onThreadStop()
{
    qDebug() << QString("subscriber #%1 is stopping!").arg( create_getsubscriberId()() );
}

// :::::::::::::::::::::::::::::::::::::::: Работа. ::::::::::::::::::::::::::::::::::::::::

auto subscriber::create_getsubscriberId() const -> std::function<uint32_t()>
{
    return [this]() -> uint32_t
    {
        return settings_.subscriberId_;
    };
}

auto subscriber::create_checkProtoVersion() -> std::function<void(uint32_t)>
{
    return [this](uint32_t protoVersion) -> void
    {
        const uint32_t incomingCommonVer = protoVersion >> 16;           // common - 16 старших бит.
        const uint32_t incomingVer    = protoVersion & 0x0000FFFF;    // 16 младших бит.

        const uint32_t myCommonVer = static_cast<uint32_t>(SOME_NAMESPACE::PROTO_VER_COMMON);

        if ((incomingCommonVer != myCommonVer) )
        {
            emit this->netLogMessage(utils::kErrors, QString("CRITICAL ERROR: Incompatible protocol versions: incoming - %1.%2, my - %3.%4!")
                .arg(incomingCommonVer).arg(incomingVer).arg(myCommonVer).arg(myVer));
        }
    };
}

auto subscriber::create_getTime_ms() const -> std::function<int64_t(int64_t)>
{
    return [this](int64_t incomingTime) -> int64_t
    {
        return QDateTime::currentMSecsSinceEpoch();

        //return timeDelta_.getTime(incomingTime, QDateTime::currentMSecsSinceEpoch());
    };
}

auto subscriber::create_setTime_ms() -> std::function<int64_t(int64_t)>    // находим смещение от времени в приходящей телеметрии!
{
    return [this](int64_t incomingTime) -> int64_t
    {
        const int64_t myTime = QDateTime::currentMSecsSinceEpoch();
       // timeDelta_.setTime(incomingTime, myTime);

        return myTime;
    };
}

auto subscriber::create_isModeOn() const -> std::function<bool()>
{
    return [this]() -> bool
    {
        return this->getsubscriberBaseCommon()->isModeOn();
    };
}

void subscriber::onConnectionStatus(const QString& netName, bool isConnected)
{
    if ((netName != settings_.subscriberName_) || (isConnected == settings_.isConnected_))    // если не мой пост, или состояние не изменилось, то выходим.
        return;

    settings_.isConnected_ = isConnected;

    utils::MessageCategory category;
    QString message = QString("group=%1, subscriber=%2, name=%3: ")
        .arg(settings_.groupId_)
        .arg(settings_.subscriberId_)
        .arg(settings_.subscriberName_);

    if (settings_.isConnected_)
    {
        category = utils::kInfo;
        message += "CONNECTION SET!";

        pushData();    // при появлении поста в сети протолкнем в него закешированные нами данные!
    }
    else
    {
        category = utils::kErrors;
        message += "CONNECTION LOST!";
    }

    emit connectionStatus(settings_.subscriberId_, settings_.isConnected_);    // сообщаем в программу об изменении статуса соединения поста!
    emit netLogMessage(category, message);
    qDebug() << message;
}


// :::::::::::::::::::::::::::::::::::::::: Полезности. ::::::::::::::::::::::::::::::::::::::::

tech::subscriberBaseCommon* subscriber::getsubscriberBaseCommon() const
{
    return p_subscriberCommon_;
}

tech::subscriberBasePAR* subscriber::getsubscriberBasePAR() const
{
    return p_subscriberPAR_;
}

double subscriber::getSectorWidth() const
{
    return settings_.workingSector_deg_;
}

bool subscriber::isMysubscriber(uint32_t subscriberId) const
{
    return create_getsubscriberId()() == subscriberId;
}
