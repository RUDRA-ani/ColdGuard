
#ifndef COLDGUARD_CONFIG_H
#define COLDGUARD_CONFIG_H

#include <Arduino.h>

namespace ColdGuard
{
namespace Config
{
// Hardware pins
constexpr uint8_t TEMPERATURE_BUS_PIN = 2;
constexpr uint8_t DOOR_SENSOR_PIN = 3;
constexpr uint8_t BUZZER_PIN = 4;
constexpr uint8_t ALARM_LED_PIN = 5;

constexpr uint8_t SD_CS_PIN = 10;

// Timing intervals
constexpr unsigned long SENSOR_SAMPLE_INTERVAL_MS = 2000UL;
constexpr unsigned long LOG_INTERVAL_MS = 10000UL;
constexpr unsigned long DOOR_OPEN_DELAY_MS = 30000UL;
constexpr unsigned long ALARM_CONFIRMATION_MS = 10000UL;

// Temperature limits (initial demonstration values).
// Change these for the actual storage application.
constexpr float MIN_ALLOWED_TEMP_C = 2.0f;
constexpr float MAX_ALLOWED_TEMP_C = 8.0f;
constexpr float TEMPERATURE_HYSTERESIS_C = 0.5f;

// Three expected DS18B20 probes.
constexpr uint8_t EXPECTED_SENSOR_COUNT = 3;

// Serial
constexpr unsigned long SERIAL_BAUD_RATE = 115200UL;
} // namespace Config
} // namespace ColdGuard

#endif
