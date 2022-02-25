#ifndef ABSTRACTHELPER_H
#define ABSTRACTHELPER_H

#include <QString>
#include <utility>
#include <functional>

namespace ProtocolSupportHelper
{
template<class PackType> std::pair<QString, QString> getPackInfoText(const PackType& pack);

namespace _details
{
    inline bool mustStop(const int counter);     // помогатор для VarPack-ов, чтобы они не писали тысячи элементов в лог.

    template<class Element, class InputIt>
    QString vdataToString(InputIt begin, InputIt end, std::function<QString(const Element&)> elementToString);
}

}

template<class PackType>
std::pair<QString, QString> ProtocolSupportHelper::getPackInfoText(const PackType& pack)
{
    return std::pair<QString, QString>("Pack", "");
}



inline bool ProtocolSupportHelper::_details::mustStop(const int counter)    // помогатор для VarPack-ов, чтобы они не писали тысячи элементов в лог.
{
    const int _LIMITER_ = 10;

    return (counter > _LIMITER_);
}

template<class Element, class InputIt>
QString ProtocolSupportHelper::_details::vdataToString(InputIt begin, InputIt end, std::function<QString(const Element&)> elementToString)
{
    QString result;

    int timeout = 0;
    for (InputIt it = begin; it < end; it++)
    {
        result += (elementToString(*it) + " // ");

        if(mustStop(++timeout))
            break;
    }

    return result;
}

#endif // ABSTRACTHELPER_H
