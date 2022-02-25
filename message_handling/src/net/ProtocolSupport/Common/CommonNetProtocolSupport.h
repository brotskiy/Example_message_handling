#ifndef COMMONNETPROTOCOLSUPPORT_H
#define COMMONNETPROTOCOLSUPPORT_H

// включайте либо выключайте оборачивание данных в std::shared_ptr!!!
#define PSUP_USE_SHRD_PTR
#include "AbstractNetProtocolSupport.h"

class CommonNetProtocolSupport : public AbstractNetProtocolSupport
{
    Q_OBJECT

// :::::::::::::::::::::::::::::::::::::::: Создание. ::::::::::::::::::::::::::::::::::::::::

public:
    CommonNetProtocolSupport(const CommonNetProtocolSupport::Settings& sttngs, ::NET_NAMESPACE::NET_CLASS* p_netWorker, QObject* parent = nullptr);

    ~CommonNetProtocolSupport() override = default;

// :::::::::::::::::::::::::::::::::::::::: Переопределяемое. ::::::::::::::::::::::::::::::::::::::::

protected:
    void registerTypes() override;
    void registerHandlers() override;

// :::::::::::::::::::::::::::::::::::::::: Сигналы: из сети в программу. ::::::::::::::::::::::::::::::::::::::::

signals: void proto_common_sigErrorsPack( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::Errors_struct ) );
signals: void proto_common_sigTelemetryPack( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::Telemetry_const_struct ), PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::Telemetry_var_struct > ) );
signals: void proto_common_sigTerminateCommandPack( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::TerminateCommand_struct ) );
signals: void proto_common_sigCurrentCoursePack( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::CurrentCourse_struct ) );

signals: void proto_common_sigTargetOnTrackPack( PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::posts::TargetOnTrack_struct > ) );
signals: void proto_common_sigCurrentSubsystemPositionPack_psts( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::posts::SubsystemPosition_struct ) );
signals: void proto_common_sigResetPack_psts( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::posts::PostID_struct ) );
signals: void proto_common_sigSetModeOnOffPack_psts( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::ModeOnOff_struct ) );
signals: void proto_common_sigGetCurrentSubsystemPositionPack_psts( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::posts::PostID_struct ) );
signals: void proto_common_sigInitialPointPack( PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::posts::InitialPoint_struct > ) );

signals: void proto_common_sigSetModeOnOffPack_grps( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::ModeOnOff_struct ) );
signals: void proto_common_sigGetTopologyPack();
signals: void proto_common_sigTopologyPack( PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::groups::SubsystemGroup_struct > ) );
signals: void proto_common_sigSetPostUseStatePack( PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::groups::UsePost_struct > ) );
signals: void proto_common_sigGetCurrentSubsystemPositionPack_grps( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::groups::GroupID_struct ) );
signals: void proto_common_sigCurrentSubsystemPositionPack_grps( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::groups::GroupID_struct ), PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::posts::SubsystemPosition_struct > ) );
signals: void proto_common_sigTrackResetPack( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::groups::TrackReset_struct ) );
signals: void proto_common_sigResetPack_grps( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::groups::GroupID_struct ), PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::posts::PostID_struct > ) );
signals: void proto_common_sigParkingPack( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::groups::GroupID_struct ) );
signals: void proto_common_sigDisableSectorPack( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::groups::GroupID_struct ), PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::posts::DisableSector_struct > ) );

// :::::::::::::::::::::::::::::::::::::::: Обработчики пачек. ::::::::::::::::::::::::::::::::::::::::

    PSUP_HNDLR( MESSAGE_PROTOCOL, ErrorsPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL , TelemetryPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL , TerminateCommandPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL, CurrentCoursePack )

    PSUP_HNDLR( MESSAGE_PROTOCOL::posts , TargetOnTrackPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL::posts, CurrentSubsystemPositionPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL::posts, ResetPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL::posts, SetModeOnOffPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL::posts, GetCurrentSubsystemPositionPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL::posts, InitialPointPack )

    PSUP_HNDLR( MESSAGE_PROTOCOL::groups, SetModeOnOffPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL::groups, GetTopologyPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL::groups, TopologyPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL::groups, SetPostUseStatePack )
    PSUP_HNDLR( MESSAGE_PROTOCOL::groups , GetCurrentSubsystemPositionPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL::groups, CurrentSubsystemPositionPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL::groups, TrackResetPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL::groups, ResetPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL::groups, ParkingPack )
    PSUP_HNDLR( MESSAGE_PROTOCOL::groups, DisableSectorPack )

// :::::::::::::::::::::::::::::::::::::::: Слоты: из программы в сеть. ::::::::::::::::::::::::::::::::::::::::

public slots: void proto_common__onErrorsPack( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::Errors_struct ) p_cdata );
public slots: void proto_common__onTelemetryPack( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::Telemetry_const_struct ) p_cdata, PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::Telemetry_var_struct > ) p_vdata );public slots: void proto_common__onTerminateCommandPack( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::TerminateCommand_struct ) p_cdata );
public slots: void proto_common__onCurrentCoursePack( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::CurrentCourse_struct ) p_cdata );

// ----------------------------------------------------------------------------------------------------------------------------------------------------

public slots: void proto_common__onTargetOnTrackPack( PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::posts::TargetOnTrack_struct > ) p_vdata );
public slots: void proto_common__onCurrentSubsystemPositionPack_psts( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::posts::SubsystemPosition_struct ) p_cdata );
public slots: void proto_common__onResetPack_psts( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::posts::PostID_struct ) p_cdata );
public slots: void proto_common__onSetModeOnOffPack_psts( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::ModeOnOff_struct ) p_cdata );
public slots: void proto_common__onGetCurrentSubsystemPositionPack_psts( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::posts::PostID_struct ) p_cdata );
public slots: void proto_common__onInitialPointPack( PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::posts::InitialPoint_struct > ) p_vdata );

// ----------------------------------------------------------------------------------------------------------------------------------------------------

public slots: void proto_common__onSetModeOnOffPack_grps( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::ModeOnOff_struct ) p_cdata );
public slots: void proto_common__onGetTopologyPack();
public slots: void proto_common__onTopologyPack( PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::groups::SubsystemGroup_struct > ) p_vdata );
public slots: void proto_common__onSetPostUseStatePack( PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::groups::UsePost_struct > ) p_vdata );
public slots: void proto_common__onGetCurrentSubsystemPositionPack_grps( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::groups::GroupID_struct ) p_cdata );
public slots: void proto_common__onCurrentSubsystemPositionPack_grps( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::groups::GroupID_struct ) p_cdata, PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::posts::SubsystemPosition_struct > ) p_vdata );
public slots: void proto_common__onTrackResetPack( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::groups::TrackReset_struct ) p_cdata );
public slots: void proto_common__onResetPack_grps( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::groups::GroupID_struct ) p_cdata, PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::posts::PostID_struct > ) p_vdata );
public slots: void proto_common__onParkingPack( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::groups::GroupID_struct ) p_cdata );
public slots: void proto_common__onDisableSectorPack( PSUP_TYPE_WRPR( MESSAGE_PROTOCOL::groups::GroupID_struct ) p_cdata, PSUP_TYPE_WRPR( std::vector< MESSAGE_PROTOCOL::posts::DisableSector_struct > ) p_vdata );

// :::::::::::::::::::::::::::::::::::::::: Служебное. :::::::::::::::::::::::::::::::::::::::

private:
    static AbstractNetProtocolSupport::Settings toAbstractSettings(const CommonNetProtocolSupport::Settings& sttngs);
    static CommonNetProtocolSupport::Settings fromAbstractSettings(const AbstractNetProtocolSupport::Settings& sttngs);

public:
    CommonNetProtocolSupport::Settings getCurrentSettings() const;

// :::::::::::::::::::::::::::::::::::::::: Ограничения. ::::::::::::::::::::::::::::::::::::::::

private:
    CommonNetProtocolSupport()                                           = delete;
    CommonNetProtocolSupport(const CommonNetProtocolSupport&)            = delete;
    CommonNetProtocolSupport(CommonNetProtocolSupport&&)                 = delete;
    CommonNetProtocolSupport& operator=(const CommonNetProtocolSupport&) = delete;
    CommonNetProtocolSupport& operator=(CommonNetProtocolSupport&&)      = delete;
};



inline AbstractNetProtocolSupport::Settings CommonNetProtocolSupport::toAbstractSettings(const CommonNetProtocolSupport::Settings& sttngs)
{
    AbstractNetProtocolSupport::Settings a_set;
    a_set.partnerNetName_ = sttngs.partnerNetName_;
    a_set.workThread_     = sttngs.workThread_;
    a_set.logPeriod_ms_   = sttngs.logPeriod_ms_;

    return a_set;
}

inline CommonNetProtocolSupport::Settings CommonNetProtocolSupport::fromAbstractSettings(const AbstractNetProtocolSupport::Settings& sttngs)
{
    CommonNetProtocolSupport::Settings c_set;
    c_set.partnerNetName_ = sttngs.partnerNetName_;
    c_set.workThread_     = sttngs.workThread_;
    c_set.logPeriod_ms_   = sttngs.logPeriod_ms_;

    return  c_set;
}

inline CommonNetProtocolSupport::Settings CommonNetProtocolSupport::getCurrentSettings() const
{
    const AbstractNetProtocolSupport::Settings a_set = AbstractNetProtocolSupport::getCurrentSettings();

    return fromAbstractSettings(a_set);
}

#endif // COMMONNETPROTOCOLSUPPORT_H
