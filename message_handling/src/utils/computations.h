#ifndef COMPUTATIONS_H
#define COMPUTATIONS_H

#include <cstdlib>
#include <cmath>
#include <type_traits>
#include <iterator>

#include <vector>

namespace computations
{
    template<class U>
    U normalizeAngle(U angle, U max)     // нормализовать угол относительно заданной величины.
    {
        while (angle >= max)
            angle -= max;

        while (angle < 0)
            angle += max;

        return angle;
    }

    template<class U>
    bool azimuthIsBetween(U from, U az, U to)    // лежит ли нормализованный азимут в заданных пределах (допускаются секторы шире 180 градусов).
    {
        if (from <= to)
            return (az >= from) && (az <= to);
        else
            return (az >= from) || (az <= to);
    }


    template<class Iter_type>
    double centralAngle(Iter_type Beg_, Iter_type End_)    // на самом деле получаемый угол не центральный, а соответствующий среднему значению синусов и косинусов!
    {
        if (Beg_ == End_)   // контейнер не должен быть пустым!
            return 0.0;

        const double size = static_cast<double>(std::abs(std::distance(Beg_, End_)));  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        double avgSin = 0;
        double avgCos = 0;

        for (auto iter = Beg_; iter != End_; iter++)
        {
            avgSin += std::sin(static_cast<double>(*iter) / 180.0 * M_PI);     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            avgCos += std::cos(static_cast<double>(*iter) / 180.0 * M_PI);
        }

        avgSin /= size;     // не должно быть 0!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        avgCos /= size;

        return (atan2(avgSin, avgCos) / M_PI * 180.0);     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }

    template<class Iter_type>
    double angleSCO(Iter_type Beg_, Iter_type End_, double centralAngle)
    {
        if (Beg_ == End_)
            return 0.0;

        const auto realSize = std::abs(std::distance(Beg_, End_));
        const double scoSize = static_cast<double>((realSize >= 2) ? (realSize - 1) : 1);  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        double sco_res = 0;

        for (auto iter = Beg_; iter != End_; iter++)
        {
            const double addend = std::pow(absAngleDiff(static_cast<double>(*iter), centralAngle), 2) / scoSize;
            sco_res += addend;
        }

        return std::sqrt(sco_res);      // получаем СКО!
    }



}


#endif // COMPUTATIONS_H
