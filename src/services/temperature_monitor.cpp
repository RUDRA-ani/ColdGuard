
#include "temperature_monitor.h"

#include "coldguard/config.h"

namespace ColdGuard
{
namespace Services
{

void TemperatureMonitor::begin()
{
    temperatureAlarm_ = false;
    sensorFault_ = true;
    outsideRange_ = false;

    violationTiming_ = false;
    violationStartedAtMs_ = 0;
}

void TemperatureMonitor::update(
    const Drivers::TemperatureReading *readings,
    uint8_t count)
{
    const uint8_t expectedCount =
        Config::EXPECTED_SENSOR_COUNT;

    // Reject unusable input without clearing an existing alarm.
    if (readings == nullptr || count > expectedCount)
    {
        sensorFault_ = true;
        temperatureAlarm_ = outsideRange_;
        return;
    }

    // Fewer sensors than expected is a fault, but the available
    // readings must still be evaluated.
    bool allReadingsValid = (count == expectedCount);
    bool anyOutsideRange = false;

    for (uint8_t i = 0; i < count; ++i)
    {
        if (!readings[i].valid)
        {
            allReadingsValid = false;
            continue;
        }

        const float temp = readings[i].celsius;

        if (temp < Config::MIN_ALLOWED_TEMP_C ||
            temp > Config::MAX_ALLOWED_TEMP_C)
        {
            anyOutsideRange = true;
        }
    }

    sensorFault_ = !allReadingsValid;

    const unsigned long now = millis();

    // Once an alarm is active, keep it active until ALL expected
    // probes are valid and inside the recovery range.
    if (outsideRange_)
    {
        bool allWithinRecoveryRange = allReadingsValid;

        for (uint8_t i = 0; i < count; ++i)
        {
            if (!readings[i].valid)
            {
                allWithinRecoveryRange = false;
                break;
            }

            const float temp = readings[i].celsius;

            if (temp <
                    Config::MIN_ALLOWED_TEMP_C +
                    Config::TEMPERATURE_HYSTERESIS_C ||
                temp >
                    Config::MAX_ALLOWED_TEMP_C -
                    Config::TEMPERATURE_HYSTERESIS_C)
            {
                allWithinRecoveryRange = false;
                break;
            }
        }

        if (allWithinRecoveryRange)
        {
            outsideRange_ = false;
            violationTiming_ = false;
        }
    }
    else
    {
        // Evaluate every valid reading even when another probe fails.
        if (anyOutsideRange)
        {
            if (!violationTiming_)
            {
                violationTiming_ = true;
                violationStartedAtMs_ = now;
            }

            if (now - violationStartedAtMs_ >=
                Config::ALARM_CONFIRMATION_MS)
            {
                outsideRange_ = true;
                violationTiming_ = false;
            }
        }
        else if (allReadingsValid)
        {
            // Reset the pending violation only when every expected
            // probe is valid and none is outside the allowed range.
            violationTiming_ = false;
        }
        // If data is incomplete, don't treat missing readings as proof
        // that a pending temperature violation has disappeared.
    }

    temperatureAlarm_ = outsideRange_;
}

bool TemperatureMonitor::hasTemperatureAlarm() const
{
    return temperatureAlarm_;
}

bool TemperatureMonitor::hasSensorFault() const
{
    return sensorFault_;
}

} // namespace Services
} // namespace ColdGuard
