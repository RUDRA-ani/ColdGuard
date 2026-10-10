
#include "rtc_clock.h"
#include <stdio.h>

namespace ColdGuard
{
namespace Drivers
{
bool RtcClock::begin()
{
    ready_ = rtc_.begin();

    if (!ready_)
    {
        return false;
    }

    // Set the RTC to the simulator/system build time if it has
    // stopped. Do not reset the clock on every reboot.
    if (!rtc_.isrunning())
    {
        rtc_.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    return true;
}

bool RtcClock::isReady() const
{
    return ready_;
}

bool RtcClock::getTimestamp(char *buffer, size_t bufferSize)
{
    if (!ready_ || buffer == nullptr || bufferSize < 20)
    {
        return false;
    }

    const DateTime now = rtc_.now();

    const int written = snprintf(
        buffer,
        bufferSize,
        "%04u-%02u-%02u %02u:%02u:%02u",
        static_cast<unsigned>(now.year()),
        static_cast<unsigned>(now.month()),
        static_cast<unsigned>(now.day()),
        static_cast<unsigned>(now.hour()),
        static_cast<unsigned>(now.minute()),
        static_cast<unsigned>(now.second()));

    return written > 0 &&
           static_cast<size_t>(written) < bufferSize;
}
} // namespace Drivers
} // namespace ColdGuard
