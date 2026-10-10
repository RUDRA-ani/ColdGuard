
#include "temperature_sensor.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#include "coldguard/config.h"

namespace
{
OneWire oneWire(ColdGuard::Config::TEMPERATURE_BUS_PIN);
DallasTemperature sensors(&oneWire);

constexpr float MIN_PLAUSIBLE_TEMP_C = -55.0f;
constexpr float MAX_PLAUSIBLE_TEMP_C = 125.0f;
}

namespace ColdGuard
{
namespace Drivers
{
bool TemperatureSensor::begin()
{
    sensorCount_ = 0;
    conversionPending_ = false;
    hasRequestedConversion_ = false;

    for (uint8_t i = 0; i < MAX_SENSORS; ++i)
    {
        temperatures_[i] = 0.0f;
        valid_[i] = false;
    }

    sensors.begin();
    sensors.setResolution(12);
    sensors.setWaitForConversion(false);

    const uint8_t detected = sensors.getDeviceCount();

    sensorCount_ = detected < MAX_SENSORS
                       ? detected
                       : MAX_SENSORS;

    return sensorCount_ > 0;
}

bool TemperatureSensor::update()
{
    const unsigned long now = millis();

    // Stage 1: collect results after conversion has completed.
    if (conversionPending_)
    {
        if (now - conversionStartedAtMs_ < CONVERSION_TIME_MS)
        {
            return false;
        }

        for (uint8_t i = 0; i < sensorCount_; ++i)
        {
            const float value = sensors.getTempCByIndex(i);

            const bool isValid =
                value != DEVICE_DISCONNECTED_C &&
                value >= MIN_PLAUSIBLE_TEMP_C &&
                value <= MAX_PLAUSIBLE_TEMP_C;

            temperatures_[i] = value;
            valid_[i] = isValid;
        }

        conversionPending_ = false;
        return true;
    }

    // Stage 2: start a new conversion when due.
    if (!hasRequestedConversion_ ||
        now - lastRequestAtMs_ >=
            ColdGuard::Config::SENSOR_SAMPLE_INTERVAL_MS)
    {
        sensors.requestTemperatures();

        conversionStartedAtMs_ = now;
        lastRequestAtMs_ = now;

        conversionPending_ = true;
        hasRequestedConversion_ = true;
    }

    return false;
}

uint8_t TemperatureSensor::sensorCount() const
{
    return sensorCount_;
}

TemperatureReading TemperatureSensor::reading(uint8_t index) const
{
    if (index >= sensorCount_)
    {
        return {0.0f, false};
    }

    return {temperatures_[index], valid_[index]};
}
}
}
