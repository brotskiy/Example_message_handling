
#include <QDebug>

#include <algorithm>
#include <iterator>
#include <utility>
#include <cstring>
#include <set>

Q_DECLARE_METATYPE(uint32_t)
Q_DECLARE_METATYPE(Post::Settings)

// :::::::::::::::::::::::::::::::::::::::: Создание. ::::::::::::::::::::::::::::::::::::::::

Group* Group::create(const QString& groupConfigPath,
                                     const QString& netConfigPath,
                                     const QString& groupLogsDir,
                                     QObject* parent)
{
    const auto groupFactory = [groupConfigPath, netConfigPath, groupLogsDir, parent]() -> Group*
    {
        Group::Settings settings;
        const bool wasRead = GroupConfiger(groupConfigPath).readGroupConfig(settings);

        if (not wasRead || not settings.isActive_)    // выходим, если не удалось прочитать конфиг, или группа выключена.
            return nullptr;

        return new Group(settings, groupConfigPath, netConfigPath, groupLogsDir, parent);
    };

    Group* me = tech::GroupBase::create<Group>(
        groupConfigPath,
        netConfigPath,
        groupFactory,
        &Group::createDefaultGroupConfig,
        &Group::createDefaultGroupNetConfig);

    return me;
}

NET_NAMESPACE::NET_CLASS* Group::createNetWorker(const QString &netConfigPath)
{
    using NET_NAMESPACE::NET_CLASS;

    NET_CLASS* p_netWorker = new NET_CLASS(this);
    p_netWorker->readConfig(netConfigPath);

    p_netWorker->setNetworkErrorHandler([this](int code, const QString& from)      // если произошел разрыв, то незамедлительно сообщаем об этом!
    {
        emit this->netErrorOccured(from);

        this->emitNetLogMessage(utils::kErrors, QString("NET ERROR: code %1 from %2 (groupId = %3)")
            .arg(code)
            .arg(from)
            .arg( this->create_getGroupId()() ));
    });

    p_netWorker->setUnknownPackHandler([this](const NET_NAMESPACE::PackDataVector& bytes)
    {
        this->emitNetLogMessage(utils::kErrors, QString("NET ERROR: get unknown pack! pack-type: %1")
            .arg(NET_NAMESPACE::packType(bytes.data())));

        return true;
    });

    p_netWorker->startNetwork();   // стартуем сеть!

    return p_netWorker;
}

void Group::createsubscribers(const Group::Settings &grpSttngs, const QString& netConfigPath)
{
    using NET_NAMESPACE::NET_CLASS;

    NET_CLASS*const p_netWorker = createNetWorker(netConfigPath);

    const auto create = [p_netWorker, this](const subscriber::Settings& subscriberSttngs) -> void
    {
        const auto factorysubscriber = [p_netWorker, subscriberSttngs, this]() -> subscriber*
        {
            return this->createsubscriber(subscriberSttngs, p_netWorker, utils::heavy::IThreadPlaceable::NO_PARENT);
        };

        subscriber*const subscriber = utils::heavy::create<subscriber>(this, factorysubscriber);
        id_to_p_subscriber_[subscriberSttngs.subscriberId_] = subscriber;

        this->connectsubscriber(subscriber);    // устанавливаем соединения между данной группой и её текущим постом.
    };

    std::for_each(grpSttngs.subscribers_.cbegin(), grpSttngs.subscribers_.cend(), create);
}

auto Group::createsubscriber(const subscriber::Settings& subscriberSttngs, NET_NAMESPACE::NET_CLASS* p_netWorker, QObject* subscriberParent) -> subscriber*
{
    using CommonSupport = CommonNetProtocolSupport;

    CommonSupport* protoCommon = subscriber::createCommonNetProtocolSupport(subscriberSttngs, p_netWorker, this);    // у поддержки протокола родителем всегда является данная группа!

    connect(this, &Group::netErrorOccured, protoCommon,
    [protoCommon](const QString& from) -> void
    {
        emit protoCommon->connectionStatus(from, false);
    });

    return new subscriber(subscriberSttngs, protoCommon, subscriberParent); 
}
