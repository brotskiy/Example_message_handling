#ifndef COMMONHELPER_H
#define COMMONHELPER_H

#include <QString>
#include <utility>

#include "AbstractHelper.h"

namespace ProtocolSupportHelper
{
    template<> std::pair<QString, QString> getPackInfoText(const TelemetryPack & pack);

}

#endif // COMMONHELPER_H
