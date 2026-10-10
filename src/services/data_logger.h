
#ifndef COLDGUARD_DATA_LOGGER_H
#define COLDGUARD_DATA_LOGGER_H

#include <Arduino.h>

#include "drivers/temperature_sensor.h"
#include "drivers/rtc_clock.h"
#include "drivers/storage.h"

namespace ColdGuard
{
namespace Services
{
class DataLogger
{
public:
    bool begin();

    void update(
        const Drivers::TemperatureReading *readings,
        uint8_t count);

    bool isReady() const;
    bool lastWriteSucceeded() const;

private:
    Drivers::RtcClock clock_;
    Drivers::Storage storage_;

    bool ready_ = false;
    bool lastWriteSucceeded_ = false;
    unsigned long lastLogAtMs_ = 0;
};
} // namespace Services
} // namespace ColdGuard

#endif
