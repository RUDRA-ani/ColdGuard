
#ifndef COLDGUARD_TEMPERATURE_MONITOR_H
#define COLDGUARD_TEMPERATURE_MONITOR_H

#include <Arduino.h>
#include "drivers/temperature_sensor.h"

namespace ColdGuard
{
namespace Services
{

class TemperatureMonitor
{
public:
    void begin();

    void update(
        const Drivers::TemperatureReading *readings,
        uint8_t count);

    bool hasTemperatureAlarm() const;
    bool hasSensorFault() const;

private:
    bool temperatureAlarm_ = false;
    bool sensorFault_ = true;
    bool outsideRange_ = false;

    bool violationTiming_ = false;
    unsigned long violationStartedAtMs_ = 0;
};

} // namespace Services
} // namespace ColdGuard

#endif
