
#include <QObject>
#include <QString>

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include <map>
#include <utility>
#include <optional>

class subscriber : public QObject, private tech::subscriberBase, public utils::heavy::IThreadPlaceable
{
    Q_OBJECT

// :::::::::::::::::::::::::::::::::::::::: Содержимое. ::::::::::::::::::::::::::::::::::::::::

public:
    struct Settings : private tech::subscriberBase::Settings
    {
        using tech::subscriberBase::Settings::subscriberId_;
        using tech::subscriberBase::Settings::subscriberName_;
        using tech::subscriberBase::Settings::position_;

        uint32_t groupId_;
        int netProtocolLogPeriod_ms_;

        Settings();
    };

    subscriber::Settings getCurrentSettings() const;

private:
    struct __Settings
    {
        const uint32_t subscriberId_;
        const uint32_t groupId_;
        const QString subscriberName_;
        const int netProtocolLogPeriod_ms_;
        SOME_NAMESPACE::subscribers::SubsystemPosition_struct position_;

        bool isConnected_ = false;

        __Settings(const subscriber::Settings& subscriberSttngs);
    } settings_;

    tech::subscriberBaseCommon* p_subscriberCommon_ = nullptr;    // логика поддержки постом протокола Common. за уничтожение отвечает объект-родитель!

    CommonNetProtocolSupport*const p_protoCommon_ = nullptr;    // ПРОСТО УКАЗАТЕЛЬ на объект оболочки над сетью для Common протокола, выполняющей прием/отправку пачек.

    struct
    {
        std::shared_ptr<tech::helper::DataContainer<SOME_NAMESPACE::PresetID_struct, SOME_NAMESPACE::Preset_struct>> p_presets_;             // получаю от поста.
        
		std::optional<SOME_NAMESPACE::groups::Usesubscriber_struct> usesubscriber_;
    } data_;

// :::::::::::::::::::::::::::::::::::::::: Создание. ::::::::::::::::::::::::::::::::::::::::

public:
    subscriber(const subscriber::Settings& subscriberSttngs,
                 CommonNetProtocolSupport* p_commonProtocol,
                 QObject* parent = utils::heavy::IThreadPlaceable::NO_PARENT);

    ~subscriber() override = default;
    void onThreadStop() override;

private:
    void createWorkData();
    void registerTypes();
    void createsubscriberBase();
    void createsubscriberBaseCommon();
    void createConnections();

public:
    static CommonNetProtocolSupport* createCommonNetProtocolSupport(const subscriber::Settings& subscriberSttngs,
                                                                    NET_LIB::NET_LIB_WORKER* p_netWorker,
                                                                    QObject* parent);

// :::::::::::::::::::::::::::::::::::::::: Работа. ::::::::::::::::::::::::::::::::::::::::

private:
    auto create_getsubscriberId()        const -> std::function<uint32_t()>;
    auto create_anysubscriberIsAllowed() const -> std::function<bool(uint32_t)>;

    auto create_isModeOn()             const -> std::function<bool()>;
    auto create_getsubscriberPosition()      const -> std::function<SOME_NAMESPACE::subscribers::SubsystemPosition_struct()>;
    auto create_setPresets()                 -> std::function<bool(const SOME_NAMESPACE::PresetID_struct&, const std::vector<SOME_NAMESPACE::Preset_struct>&)>;
    auto create_getPresets()           const -> std::function<std::pair<SOME_NAMESPACE::PresetID_struct, std::vector<SOME_NAMESPACE::Preset_struct>>()>;
   
public slots:  void onSettingsRequst(uint32_t subscriberId);
signals:       void sigCurrentSettings(const subscriber::Settings&);

private slots: void onConnectionStatus(const QString& netName, bool isConnected);
signals:       void connectionStatus(uint32_t subscriberId, bool isConnected);
private:       void pushData();    // запросить/послать в пост сведения при появлении его в сети.

public slots:  void onClearCache(uint32_t subscriberId);

// :::::::::::::::::::::::::::::::::::::::: Полезности. ::::::::::::::::::::::::::::::::::::::::

signals: void netLogMessage(utils::MessageCategory category, const QString& text);

public:
    tech::subscriberBaseCommon* getsubscriberBaseCommon() const;

private:
    bool isMysubscriber(uint32_t subscriberId) const;

// :::::::::::::::::::::::::::::::::::::::: Ограничения. ::::::::::::::::::::::::::::::::::::::::

private:
    subscriber()                               = delete;
    subscriber(const subscriber&)            = delete;
    subscriber& operator=(const subscriber&) = delete;
    subscriber(subscriber&&)                 = delete;
    subscriber& operator=(subscriber&&)      = delete;
};
