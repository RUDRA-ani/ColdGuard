
#ifndef COLDGUARD_RTC_CLOCK_H
#define COLDGUARD_RTC_CLOCK_H

#include <Arduino.h>
#include <RTClib.h>

namespace ColdGuard
{
namespace Drivers
{
class RtcClock
{
public:
    bool begin();
    bool isReady() const;
    bool getTimestamp(char *buffer, size_t bufferSize);

private:
    RTC_DS1307 rtc_;
    bool ready_ = false;
};
} // namespace Drivers
} // namespace ColdGuard

#endif
