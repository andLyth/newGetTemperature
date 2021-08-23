#ifndef PRECISION_H_INCLUDED
#define PRECISION_H_INCLUDED
#endif // PRECISION_H_INCLUDED
#include <date.h>           //for time format (UTC)
#include <iomanip>          //std::setprecision
#include <chrono>           //std::chrono





template <class Precision>
std::string getISOCurrentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    return date::format("%FT%TZ", date::floor<Precision>(now));
}
