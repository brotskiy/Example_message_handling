#include "CommonHelper.h"

#include <cstdint>


template<>
std::pair<QString, QString> ProtocolSupportHelper::getPackInfoText(const TelemetryPack & pack)
{
    const QString datatext = QString("grp=%1, p=%2, isOn=%3")
        .arg(pack.m_CData.group_id_)
        .arg(pack.m_CData.p)
        .arg(pack.m_CData.workMode_ == kOnMode);

    return std::pair<QString, QString>("TelemetryPack", datatext);
}
