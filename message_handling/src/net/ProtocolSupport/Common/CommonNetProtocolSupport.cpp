#include "CommonNetProtocolSupport.h"

Q_DECLARE_METATYPE( PSUP_TYPE_WRPR( STRUCT_NAMESPACE::Errors_struct ) )
Q_DECLARE_METATYPE( PSUP_TYPE_WRPR( STRUCT_NAMESPACE::Telemetry_const_struct ) )

// :::::::::::::::::::::::::::::::::::::::: Создание. ::::::::::::::::::::::::::::::::::::::::

CommonNetProtocolSupport::CommonNetProtocolSupport(const CommonNetProtocolSupport::Settings& sttngs, NET_LIB::NET_LIB_WORKER* p_netWorker, QObject* parent) :
    AbstractNetProtocolSupport(toAbstractSettings(sttngs), p_netWorker, parent)
{
    registerTypes();
    registerHandlers();
}

// :::::::::::::::::::::::::::::::::::::::: Переопределяемое. ::::::::::::::::::::::::::::::::::::::::

void CommonNetProtocolSupport::registerTypes()
{
    // тип каждого аргумента ДОЛЖЕН быть ОБЕРНУТ В МАКРОС для поддержки унификации!!!
    qRegisterMetaType<PSUP_TYPE_WRPR( STRUCT_NAMESPACE::Errors_struct )>();
    qRegisterMetaType<PSUP_TYPE_WRPR( STRUCT_NAMESPACE::Telemetry_const_struct )>();
}

void CommonNetProtocolSupport::registerHandlers()
{
    registerHandler<PACK_NAMESPACE::ErrorsPack>(this, &CommonNetProtocolSupport::proto_hndlr_ErrorsPack );
    registerHandler<PACK_NAMESPACE::TelemetryPack>(this, &CommonNetProtocolSupport::proto_hndlr_TelemetryPack );
}

// :::::::::::::::::::::::::::::::::::::::: Обработчики сетевых пачек. ::::::::::::::::::::::::::::::::::::::::

// получив сетевую пачку, генерируем сигнал, испукаемый внутрь программы.

 void CommonNetProtocolSupport::proto_hndlr_ErrorsPack(const PACK_NAMESPACE::ErrorsPack &pack)
{
auto&& p_cdata = extractCData(pack.m_CData);   // FORWARDING reference!
emit this->proto_common_sigErrorsPack(p_cdata);
}

 void CommonNetProtocolSupport::proto_hndlr_TelemetryPack(const PACK_NAMESPACE::TelemetryPack &pack)
{
auto&& p_cdata = extractCData(pack.m_CData);   // FORWARDING reference!
auto&& p_vdata = extractVData(pack.m_VData);   // FORWARDING reference!
emit this->proto_common_sigTelemetryPack(p_cdata, p_vdata);
}

// :::::::::::::::::::::::::::::::::::::::: Слоты: из программы в сеть. ::::::::::::::::::::::::::::::::::::::::

void CommonNetProtocolSupport::proto_common__onErrorsPack( PSUP_TYPE_WRPR( PACK_NAMESPACE::Errors_struct ) p_cdata )
{
proto__on_ConstPack<PACK_NAMESPACE::ErrorsPack>(p_cdata);
}

void CommonNetProtocolSupport::proto_common__onTelemetryPack( PSUP_TYPE_WRPR( STRUCT_NAMESPACE::Telemetry_const_struct ) p_cdata, 
															  PSUP_TYPE_WRPR( std::vector< STRUCT_NAMESPACE::Telemetry_var_struct > ) p_vdata )
{
proto__on_ConstVarPack<PACK_NAMESPACE::TelemetryPack>(p_cdata, p_vdata);
}
