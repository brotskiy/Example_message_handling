#include "AbstractNetProtocolSupport.h"

#include <cstdlib>
#include <cmath>

Q_DECLARE_METATYPE( utils::MessageCategory )

// :::::::::::::::::::::::::::::::::::::::: Удобства и объявления. ::::::::::::::::::::::::::::::::::::::::

AbstractNetProtocolSupport::Settings AbstractNetProtocolSupport::getCurrentSettings() const
{
    AbstractNetProtocolSupport::Settings result;

    result.partnerNetName_ = partnerNetName_;
    result.workThread_     = workThread_;
    result.logPeriod_ms_   = logPeriod_ms_;

    return result;
}

// :::::::::::::::::::::::::::::::::::::::: Создание. ::::::::::::::::::::::::::::::::::::::::

AbstractNetProtocolSupport::AbstractNetProtocolSupport(const AbstractNetProtocolSupport::Settings& sttngs, NET_LIB::NET_LIB_WORKER* p_netWorker, QObject* parent) :
    QObject(parent),
    partnerNetName_(sttngs.partnerNetName_),
    workThread_(sttngs.workThread_),
    logPeriod_ms_(sttngs.logPeriod_ms_),
    p_netWorker_(p_netWorker)
    {
        qRegisterMetaType<utils::MessageCategory>();
    }

// :::::::::::::::::::::::::::::::::::::::: Служебное. ::::::::::::::::::::::::::::::::::::::::

void AbstractNetProtocolSupport::emitPackLogMessageImpl(int64_t curTime, int64_t& refPrevTime, const QString& message)
{
    refPrevTime = curTime;

    emit logMessage(utils::kInfo, message);
}

bool AbstractNetProtocolSupport::canEmit(int64_t curTime, int64_t prevTime) const
{
    return (std::abs(curTime - prevTime) >= logPeriod_ms_);
}
