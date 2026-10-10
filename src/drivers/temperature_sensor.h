
#ifndef COLDGUARD_TEMPERATURE_SENSOR_H
#define COLDGUARD_TEMPERATURE_SENSOR_H

#include <Arduino.h>

namespace ColdGuard
{
namespace Drivers
{
struct TemperatureReading
{
    float celsius;
    bool valid;
};

class TemperatureSensor
{
public:
    bool begin();
    bool update();

    uint8_t sensorCount() const;
    TemperatureReading reading(uint8_t index) const;

private:
    static const uint8_t MAX_SENSORS = 3;
    static const unsigned long CONVERSION_TIME_MS = 750UL;

    float temperatures_[MAX_SENSORS];
    bool valid_[MAX_SENSORS];
    uint8_t sensorCount_ = 0;

    bool conversionPending_ = false;
    bool hasRequestedConversion_ = false;

    unsigned long conversionStartedAtMs_ = 0;
    unsigned long lastRequestAtMs_ = 0;
};
}
}

#endif
