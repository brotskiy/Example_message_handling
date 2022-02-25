#ifndef ABSTRACTNETPROTOCOLSUPPORT_H
#define ABSTRACTNETPROTOCOLSUPPORT_H

#include <QObject>
#include <QString>
#include <QDateTime>

#include <memory>
#include <functional>
#include <vector>
#include <map>
#include <type_traits>
#include <utility>


#include "AbstractHelper.h"
#include "Helper1.h"


// НИКОГДА НЕ СТАВЬТЕ НАЧИНАЮЩИЙСЯ С "//" КОММЕНТАРИЙ В СТРОКУ К МАКРОСУ!!!



// ........................................ Макросы. ........................................

// Включайте либо выключайте оборачивание данных в std::shared_ptr в нужных вам файлах c помощью #define PSUP_USE_SHRD_PTR

#ifdef PSUP_USE_SHRD_PTR
    // обертка shared_ptr над данными. ОСОБОЕ ВНИМАНИЕ НА ЭТОТ МАКРОС!!!
    #define PSUP_TYPE_WRPR(AnyType) std::shared_ptr< AnyType >
#else
    // обертка над данными отсутствует. ОСОБОЕ ВНИМАНИЕ НА ЭТОТ МАКРОС!!!
    #define PSUP_TYPE_WRPR(AnyType) const AnyType &
#endif

// объявление обработчика пачки.
#define PSUP_HNDLR(PackNamespaces, PackType) protected: virtual void proto_hndlr_##PackType(const PackNamespaces :: PackType & pack );

// ........................................ Конец макросов. ........................................

class AbstractNetProtocolSupport : public QObject               // pure virtual class.
{
    Q_OBJECT

// :::::::::::::::::::::::::::::::::::::::: Удобства и объявления. ::::::::::::::::::::::::::::::::::::::::

protected:
    static const bool IWantToSendPack = true;           // служебный флаг, что я точно хочу послать пачку по сети.
    static const bool IDoNotWantToSendPack = false;     // служебный флаг, что я НЕ хочу посылать пачку по сети.

private:
    struct PackTransmissionEmittedLogTime
    {
        int64_t incoming_ms = 0;
        int64_t outcoming_ms = 0;
    };

public:
    struct Settings
    {
        QString partnerNetName_   = "YOUR_NET_PARTNER_NAME_GOES_HERE";
        HandlerThread workThread_ = _THREAD;
        int64_t logPeriod_ms_     = 0;

        Settings() {}
    };
    AbstractNetProtocolSupport::Settings getCurrentSettings() const;

// :::::::::::::::::::::::::::::::::::::::: Содержимое. ::::::::::::::::::::::::::::::::::::::::
private:
    const QString partnerNetName_;                  // сетевое имя объекта, c которым я осуществляю обмен. МОЕ ИМЯ НЕ УКАЗЫВАЕТСЯ!
    const HandlerThread workThread_;                // поток, в котором выполняется обработчик сетевой пачки.
    const int64_t logPeriod_ms_;                    // период логирования сообщений, что пришла/ушла пачка.
    NET_CLASS* p_netWorker_ = nullptr;      // получаемый ИЗВНЕ обработчик сети.

    std::map<int32_t, PackTransmissionEmittedLogTime> packTimes_;


// :::::::::::::::::::::::::::::::::::::::: Создание. ::::::::::::::::::::::::::::::::::::::::
public:
    AbstractNetProtocolSupport(const AbstractNetProtocolSupport::Settings& sttngs, NET_NAMESPACE::NET_CLASS* p_netWorker, QObject* parent = nullptr);

    virtual ~AbstractNetProtocolSupport() = default;

// :::::::::::::::::::::::::::::::::::::::: Переопределяемое. ::::::::::::::::::::::::::::::::::::::::

protected:
    virtual void registerTypes() = 0;
    virtual void registerHandlers() = 0;

 // :::::::::::::::::::::::::::::::::::::::: Поддержка логирования. ::::::::::::::::::::::::::::::::::::::::

signals:
    void logMessage(utils::MessageCategory category, const QString& text);
    void connectionStatus(const QString& netName, bool isConnected);

// :::::::::::::::::::::::::::::::::::::::: Регистрация обработчиков. ::::::::::::::::::::::::::::::::::::::::

protected:
    // получает логику работы, делает из неё обработчик и регистрирует его. перегрузка для функции-члена.
    template<class PackType, class NetSupportType>
    void registerHandler(NetSupportType* netSupportObject, void(NetSupportType::* memberWorkLogic)(const PackType&));

    // получает логику работы, делает из неё обработчик и регистрирует его. перегрузка для независимого функционального объекта.
    template<class PackType>
    void registerHandler(std::function<void(const PackType&)> workLogic);

private:
    // просто получает логику работы и оборачивает её инструкциями, общими для всех обработчиков.
    template<class PackType>
    auto createHandler(std::function<void(const PackType&)> workLogic) -> std::function<bool(const PackData&)>;


// :::::::::::::::::::::::::::::::::::::::: Логирование. ::::::::::::::::::::::::::::::::::::::::

protected: template<class PackType> void emitPackLogMessage(const PackType& pack, bool incoming);
private:                            void emitPackLogMessageImpl(int64_t curTime, int64_t& refPrevTime, const QString& message);
private:                            bool canEmit(int64_t curTime, int64_t prevTime) const;

// :::::::::::::::::::::::::::::::::::::::: Средства создания слотов. ::::::::::::::::::::::::::::::::::::::::

// отправка пачки.
protected:
    template<class PackableT>
    void sendPack(PackableT* pack, bool force = IDoNotWantToSendPack);

// реализации слотов.
protected:
    template<class PackType>
    void proto__on_EmptyPack();

    template<class PackType, class CData_t>
    void proto__on_ConstPack(PSUP_TYPE_WRPR(CData_t) p_cdata);

    template<class PackType, class VData_t>
    void proto__on_VarPack(PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata);

    template<class PackType, class CData_t, class VData_t>
    void proto__on_ConstVarPack(PSUP_TYPE_WRPR(CData_t) p_cdata, PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata);

    template<class PackType, class CData_t, class VData_t, class VVData_t>
    void proto__on_ConstVarVVarPack(PSUP_TYPE_WRPR(CData_t) p_cdata,
                                    PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata,
                                    PSUP_TYPE_WRPR(std::vector<std::vector<VVData_t>>) p_vvdata);

    template<class PackType, class VData_t, class VVData_t>
    void proto__on_VarVVarPack(PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata,
                               PSUP_TYPE_WRPR(std::vector<std::vector<VVData_t>>) p_vvdata);

private:
    template<class PackType>
    void _proto__on_EmptyPack_Impl_(PackType & pack_out);

    template<class PackType, class CData_t>
    void _proto__on_ConstPack_Impl_(PackType & pack_out, PSUP_TYPE_WRPR(CData_t) p_cdata);

    template<class PackType, class VData_t>
    void _proto__on_VarPack_Impl_(PackType & pack_out, PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata);

    template<class PackType, class CData_t, class VData_t>
    void _proto__on_ConstVarPack_Impl_(PackType & pack_out, PSUP_TYPE_WRPR(CData_t) p_cdata, PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata);

    template<class PackType, class CData_t, class VData_t, class VVData_t>
    void _proto__on_ConstVarVVarPack_Impl_(PackType & pack_out, PSUP_TYPE_WRPR(CData_t) p_cdata,
                                                                PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata,
                                                                PSUP_TYPE_WRPR(std::vector<std::vector<VVData_t>>) p_vvdata);

    template<class PackType, class VData_t, class VVData_t>
    void _proto__on_VarVVarPack_Impl_(PackType & pack_out, PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata,
                                                           PSUP_TYPE_WRPR(std::vector<std::vector<VVData_t>>) p_vvdata);

// доступ к данным.
protected:
    // получение данных ИЗ ПАЧКИ для отправки в приложение.
    template<class CDataType>
    static auto extractCData(const CDataType& cdata) -> PSUP_TYPE_WRPR(CDataType);                            // аргумент - const LEFT value ссылка на CData!

    template<class DataType>
    static auto extractVData(const std::vector<DataType>& vdata) -> PSUP_TYPE_WRPR(std::vector<DataType>);    // аргумент - const LEFT value ссылка на VData!

    // получение доступа к данным ДЛЯ ЗАПИСИ В ПАЧКУ для отправки по сети.
    template<class CDataType>
    static auto getAccessCData(PSUP_TYPE_WRPR(CDataType) cdata) -> const CDataType &;                               // ВНИМАНИЕ! возвращается const LEFT value ссылка!

    template<class DataType>
    static auto getAccessVData(PSUP_TYPE_WRPR(std::vector<DataType>) vdata) -> const std::vector<DataType> &;       // ВНИМАНИЕ! возвращается const LEFT value ссылка!
};



// ........................................ Регистрация обработчиков ........................................

template<class PackType, class NetSupportType>
void AbstractNetProtocolSupport::registerHandler(NetSupportType* netSupportObject, void(NetSupportType::* memberWorkLogic)(const PackType&))
{
    static_assert(std::is_base_of<AbstractNetProtocolSupport, NetSupportType>::value,
        "net protocol support must be a subtype of AbstractNetProtocolSupport!");

    auto workLogicLambda = [netSupportObject, memberWorkLogic](const PackType& pack) -> void    // делаем лямбду из функции-члена.
    {
        return (netSupportObject->*memberWorkLogic)(pack);      // можно делать return void!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    };

    registerHandler<PackType>(workLogicLambda);    // передаем лямбду в версию для независимой функции.
}

template<class PackType>
void AbstractNetProtocolSupport::registerHandler(std::function<void(const PackType&)> workLogic)
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't register handler for non-packable!");

    p_netWorker_->setHandler(PackType::pack_type, partnerNetName_, createHandler<PackType>(workLogic), workThread_);
}

template<class PackType>
auto AbstractNetProtocolSupport::createHandler(std::function<void(const PackType&)> workLogic) -> std::function<bool(const PackData&)>
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't create handler for non-packable!");

    auto handler = [this, workLogic](const PackData& bytes) -> bool
    {
        PackType pack(bytes.data());
        workLogic(pack);
        emit this->connectionStatus(partnerNetName_, true);    // получили пачку, значит пост в сети.
        this->emitPackLogMessage(pack, true);

        return true;
    };

    return handler;
}

// ........................................ Логирование. ........................................

template<class PackType>
void AbstractNetProtocolSupport::emitPackLogMessage(const PackType& pack, bool incoming)
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't emit message for non-packable!");

    const int32_t packType = PackType::pack_type;       // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    const int64_t curTime_ms = QDateTime::currentMSecsSinceEpoch();
    int64_t & refPrevTime_ms = (incoming)
        ? packTimes_[packType].incoming_ms
        : packTimes_[packType].outcoming_ms;

    if (not canEmit(curTime_ms, refPrevTime_ms))        // пишем в лог, только если прошел заданный промежуток времени.
        return;

    const QString textTemplate = (incoming)
        ? "get      [%1] [%2] from %3: %4"
        : "initiate [%1] [%2] to %3: %4";

    const std::pair<QString, QString> packText = ProtocolSupportHelper::getPackInfoText(pack);
    const QString message = textTemplate
        .arg(packType).arg(packText.first).arg(partnerNetName_).arg(packText.second);

    emitPackLogMessageImpl(curTime_ms, refPrevTime_ms, message);
}

// ........................................ Средства создания слотов. ........................................

template<class PackableT>
void AbstractNetProtocolSupport::sendPack(PackableT* pack, bool force)
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackableT>::value, "can't send non-packable!");

    if (!force)
        return;

    p_netWorker_->send(pack, partnerNetName_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template<class PackType>
void AbstractNetProtocolSupport::proto__on_EmptyPack()
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't create EmptyPack slot for non-packable!");

    PackType pack;
    _proto__on_EmptyPack_Impl_(pack);
    this->emitPackLogMessage(pack, false);
}

template<class PackType, class CData_t>
void AbstractNetProtocolSupport::proto__on_ConstPack(PSUP_TYPE_WRPR(CData_t) p_cdata)
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't create ConstPack slot for non-packable!");

    PackType pack;
    _proto__on_ConstPack_Impl_(pack, p_cdata);
    this->emitPackLogMessage(pack, false);
}

template<class PackType, class VData_t>
void AbstractNetProtocolSupport::proto__on_VarPack(PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata)
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't create ConstPack slot for non-packable!");

    PackType pack;
    _proto__on_VarPack_Impl_(pack, p_vdata);
    this->emitPackLogMessage(pack, false);
}

template<class PackType, class CData_t, class VData_t>
void AbstractNetProtocolSupport::proto__on_ConstVarPack(PSUP_TYPE_WRPR(CData_t) p_cdata, PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata)
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't create ConstPack slot for non-packable!");

    PackType pack;
    _proto__on_ConstVarPack_Impl_(pack, p_cdata, p_vdata);
    this->emitPackLogMessage(pack, false);
}

template<class PackType, class CData_t, class VData_t, class VVData_t>
void AbstractNetProtocolSupport::proto__on_ConstVarVVarPack(PSUP_TYPE_WRPR(CData_t) p_cdata,
                                                            PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata,
                                                            PSUP_TYPE_WRPR(std::vector<std::vector<VVData_t>>) p_vvdata)
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't create ConstVarVVarPack slot for non-packable!");

    PackType pack;
    _proto__on_ConstVarVVarPack_Impl_(pack, p_cdata, p_vdata, p_vvdata);
    this->emitPackLogMessage(pack, false);
}

template<class PackType, class VData_t, class VVData_t>
void AbstractNetProtocolSupport::proto__on_VarVVarPack(PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata,
                                                       PSUP_TYPE_WRPR(std::vector<std::vector<VVData_t>>) p_vvdata)
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't create VarVVarPack slot for non-packable!");

    PackType pack;
    _proto__on_VarVVarPack_Impl_(pack, p_vdata, p_vvdata);
    this->emitPackLogMessage(pack, false);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template<class PackType>
void AbstractNetProtocolSupport::_proto__on_EmptyPack_Impl_(PackType & pack_out)
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't create EmptyPack slot IMPLEMENTATION for non-packable!");

    sendPack(&pack_out, IWantToSendPack);
}

template<class PackType, class CData_t>
void AbstractNetProtocolSupport::_proto__on_ConstPack_Impl_(PackType & pack_out, PSUP_TYPE_WRPR(CData_t) p_cdata)
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't create ConstPack slot IMPLEMENTATION for non-packable!");

    const auto& cdata = getAccessCData(p_cdata);
    pack_out.m_CData = cdata;

    sendPack(&pack_out, IWantToSendPack);
}

template<class PackType, class VData_t>
void AbstractNetProtocolSupport::_proto__on_VarPack_Impl_(PackType & pack_out, PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata)
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't create VarPack slot IMPLEMENTATION for non-packable!");

    const auto& vdata = getAccessVData(p_vdata);
    pack_out.m_VData = vdata;

    sendPack(&pack_out, IWantToSendPack);
}

template<class PackType, class CData_t, class VData_t>
void AbstractNetProtocolSupport::_proto__on_ConstVarPack_Impl_(PackType & pack_out, PSUP_TYPE_WRPR(CData_t) p_cdata, PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata)
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't create ConstVarPack slot IMPLEMENTATION for non-packable!");

    const auto& cdata = getAccessCData(p_cdata);
    const auto& vdata = getAccessVData(p_vdata);

    pack_out.m_CData = cdata;
    pack_out.m_VData = vdata;

    sendPack(&pack_out, IWantToSendPack);
}

template<class PackType, class CData_t, class VData_t, class VVData_t>
void AbstractNetProtocolSupport::_proto__on_ConstVarVVarPack_Impl_(PackType & pack_out, PSUP_TYPE_WRPR(CData_t) p_cdata,
                                                                                        PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata,
                                                                                        PSUP_TYPE_WRPR(std::vector<std::vector<VVData_t>>) p_vvdata)
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't create ConstVarVVarPack slot IMPLEMENTATION for non-packable!");

    const auto& cdata  = getAccessCData(p_cdata);
    const auto& vdata  = getAccessVData(p_vdata);
    const auto& vvdata = getAccessVData(p_vvdata);

    pack_out.m_CData  = cdata;
    pack_out.m_VData  = vdata;
    pack_out.m_VVData = vvdata;

    sendPack(&pack_out, IWantToSendPack);
}

template<class PackType, class VData_t, class VVData_t>
void AbstractNetProtocolSupport::_proto__on_VarVVarPack_Impl_(PackType & pack_out, PSUP_TYPE_WRPR(std::vector<VData_t>) p_vdata,
                                                                                   PSUP_TYPE_WRPR(std::vector<std::vector<VVData_t>>) p_vvdata)
{
    static_assert(std::is_base_of<NET_NAMESPACE::Packable, PackType>::value, "can't create VarVVarPack slot IMPLEMENTATION for non-packable!");

    const auto& vdata  = getAccessVData(p_vdata);
    const auto& vvdata = getAccessVData(p_vvdata);

    pack_out.m_VData  = vdata;
    pack_out.m_VVData = vvdata;

    sendPack(&pack_out, IWantToSendPack);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// получение данных ИЗ ПАЧКИ для отправки в приложение.
template<class CDataType>
auto AbstractNetProtocolSupport::extractCData(const CDataType& cdata) -> PSUP_TYPE_WRPR(CDataType)
{
#ifdef PSUP_USE_SHRD_PTR
    return std::make_shared<CDataType>(cdata);
#else
    return cdata;    // просто получаю lvalue& и возвращаю её через const lvalue&. никакие действия не производятся!
#endif
}

template<class DataType>
auto AbstractNetProtocolSupport::extractVData(const std::vector<DataType>& vdata) -> PSUP_TYPE_WRPR(std::vector<DataType>)
{
#ifdef PSUP_USE_SHRD_PTR
    return std::make_shared<std::vector<DataType>>(vdata);
#else
    return vdata;    // просто получаю lvalue& и возвращаю её через const lvalue&. никакие действия не производятся!
#endif
}

// получение доступа к данным ДЛЯ ЗАПИСИ В ПАЧКУ для отправки по сети.
template<class CDataType>
auto AbstractNetProtocolSupport::getAccessCData(PSUP_TYPE_WRPR(CDataType) cdata) -> const CDataType &
{
#ifdef PSUP_USE_SHRD_PTR
    return ( *cdata.get() );
#else
    return cdata;
#endif
}

template<class DataType>
auto AbstractNetProtocolSupport::getAccessVData(PSUP_TYPE_WRPR(std::vector<DataType>) vdata) -> const std::vector<DataType> &
{
#ifdef PSUP_USE_SHRD_PTR
    return ( *vdata.get() );
#else
    return vdata;
#endif
}

#endif // ABSTRACTNETPROTOCOLSUPPORT_H
