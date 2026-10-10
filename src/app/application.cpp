 
#include "application.h"

#include <Arduino.h>
#include <stdio.h>

#include "coldguard/config.h"
#include "drivers/temperature_sensor.h"
#include "drivers/door_sensor.h"
#include "drivers/rtc_clock.h"
#include "drivers/storage.h"
#include "services/temperature_monitor.h"
#include "services/alarm_manager.h"

namespace
{
ColdGuard::Drivers::TemperatureSensor temperatureSensor;
ColdGuard::Drivers::DoorSensor doorSensor;
ColdGuard::Drivers::RtcClock rtcClock;
ColdGuard::Drivers::Storage storage;

ColdGuard::Services::TemperatureMonitor temperatureMonitor;
ColdGuard::Services::AlarmManager alarmManager;

bool sensorInitialized = false;
bool rtcInitialized = false;
bool storageInitialized = false;

unsigned long lastRetryAtMs = 0;

bool previousTemperatureAlarm = false;
bool previousSensorFault = false;
bool previousDoorOpen = false;
bool previousDoorAlarm = false;
}
// Log an event with an RTC timestamp when available.
 // Log an event with an RTC timestamp when available.
void logEvent(const char *event)
{
    char timestamp[20];
    char line[100];

    if (rtcInitialized &&
        rtcClock.getTimestamp(timestamp, sizeof(timestamp)))
    {
        snprintf(line, sizeof(line), "%s,%s", timestamp, event);
    }
    else
    {
        snprintf(line, sizeof(line), "RTC_UNAVAILABLE,%s", event);
    }

    Serial.println(line);

    if (storageInitialized)
    {
        const bool saved = storage.appendLine("events.csv", line);

        Serial.println(
            saved ? F("SD LOG: write successful")
                  : F("SD LOG: write failed")
        );
    }
}// namespace

void application_init()
{
    Serial.println(F("ColdGuard starting..."));

    alarmManager.begin();
    temperatureMonitor.begin();
    doorSensor.begin();

    // Initialize the real-time clock.
    rtcInitialized = rtcClock.begin();

    if (rtcInitialized)
    {
        Serial.println(F("RTC initialized."));
    }
    else
    {
        Serial.println(F("WARNING: RTC not detected."));
    }

    // Initialize persistent storage.
    storageInitialized = storage.begin();

    if (storageInitialized)
    {
        Serial.println(F("Storage initialized."));
    }
    else
    {
        Serial.println(F("WARNING: Storage not available."));
    }

    // Initialize temperature probes.
    sensorInitialized = temperatureSensor.begin();

    if (sensorInitialized)
    {
        Serial.print(F("Temperature probes detected: "));
        Serial.println(temperatureSensor.sensorCount());
    }
    else
    {
        Serial.println(F("WARNING: Temperature probes not detected."));
    }

    lastRetryAtMs = millis();

    logEvent("SYSTEM_STARTED");
}

void application_update()
{
    const unsigned long now = millis();

    // Always monitor the door, including during temperature conversion.
    doorSensor.update();

    const bool doorOpen = doorSensor.isOpen();

    const bool doorAlarm =
        doorSensor.hasBeenOpenFor(
            ColdGuard::Config::DOOR_OPEN_DELAY_MS);

    // Detect and log door state changes.
    if (doorOpen != previousDoorOpen)
    {
        if (doorOpen)
        {
            Serial.println(F("Door opened."));
            logEvent("DOOR_OPENED");
        }
        else
        {
            Serial.println(F("Door closed."));
            logEvent("DOOR_CLOSED");
        }

        previousDoorOpen = doorOpen;
    }

    // Detect and log door-open timeout transitions.
    if (doorAlarm != previousDoorAlarm)
    {
        if (doorAlarm)
        {
            Serial.println(
                F("ALARM: Door has been open too long!"));

            logEvent("DOOR_OPEN_TOO_LONG");
        }
        else
        {
            Serial.println(
                F("Door-open alarm cleared."));

            logEvent("DOOR_ALARM_CLEARED");
        }

        previousDoorAlarm = doorAlarm;
    }

    // Retry temperature sensor initialization every 2 seconds.
    if (!sensorInitialized)
    {
        if (now - lastRetryAtMs >= 2000UL)
        {
            lastRetryAtMs = now;
            sensorInitialized = temperatureSensor.begin();

            if (sensorInitialized)
            {
                Serial.println(
                    F("Temperature probes initialized."));

                logEvent("TEMPERATURE_PROBES_INITIALIZED");
            }
        }
    }

    // Process temperatures only when a conversion is complete.
    if (sensorInitialized && temperatureSensor.update())
    {
        ColdGuard::Drivers::TemperatureReading readings[3];

        const uint8_t count = temperatureSensor.sensorCount();

        for (uint8_t i = 0; i < count; ++i)
        {
            readings[i] = temperatureSensor.reading(i);

            Serial.print(F("Probe "));
            Serial.print(i + 1);
            Serial.print(F(": "));

            if (readings[i].valid)
            {
                Serial.print(readings[i].celsius, 2);
                Serial.println(F(" C"));
            }
            else
            {
                Serial.println(F("INVALID"));
            }
        }

        temperatureMonitor.update(readings, count);

        const bool temperatureAlarm =
            temperatureMonitor.hasTemperatureAlarm();

        const bool sensorFault =
            temperatureMonitor.hasSensorFault();

        // Log temperature alarm transitions.
        if (temperatureAlarm != previousTemperatureAlarm)
        {
            if (temperatureAlarm)
            {
                Serial.println(
                    F("ALARM: Temperature outside limits!"));

                logEvent("TEMPERATURE_ALARM");
            }
            else
            {
                Serial.println(
                    F("Temperature returned to recovery range."));

                logEvent("TEMPERATURE_ALARM_CLEARED");
            }

            previousTemperatureAlarm = temperatureAlarm;
        }

        // Log temperature sensor fault transitions.
        if (sensorFault != previousSensorFault)
        {
            if (sensorFault)
            {
                Serial.println(
                    F("FAULT: Invalid temperature sensor reading."));

                logEvent("TEMPERATURE_SENSOR_FAULT");
            }
            else
            {
                Serial.println(
                    F("Temperature sensor fault cleared."));

                logEvent("TEMPERATURE_SENSOR_FAULT_CLEARED");
            }

            previousSensorFault = sensorFault;
        }
    }

    

    // Combine all alarm causes.
    // One cleared condition must not silence another active alarm.
    const bool alarmActive =
        temperatureMonitor.hasTemperatureAlarm() ||
        temperatureMonitor.hasSensorFault() ||
        doorAlarm;

    alarmManager.setActive(alarmActive);
}
