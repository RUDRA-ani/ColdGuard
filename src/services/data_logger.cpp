
#include "data_logger.h"

#include <SD.h>
#include <stdio.h>

#include "coldguard/config.h"

namespace
{
constexpr char LOG_FILENAME[] = "/coldguard.csv";
constexpr char CSV_HEADER[] =
    "timestamp,temp1_c,temp2_c,temp3_c,sensor_fault";
}

namespace ColdGuard
{
namespace Services
{
bool DataLogger::begin()
{
    const bool clockReady = clock_.begin();
    const bool storageReady = storage_.begin();

    ready_ = clockReady && storageReady;

    if (!ready_)
    {
        lastWriteSucceeded_ = false;
        return false;
    }

    if (!SD.exists(LOG_FILENAME))
    {
        lastWriteSucceeded_ =
            storage_.appendLine(LOG_FILENAME, CSV_HEADER);

        if (!lastWriteSucceeded_)
        {
            ready_ = false;
            return false;
        }
    }

    lastLogAtMs_ = millis();
    lastWriteSucceeded_ = true;

    return true;
}

void DataLogger::update(
    const Drivers::TemperatureReading *readings,
    uint8_t count)
{
    if (!ready_ || readings == nullptr ||
        count != Config::EXPECTED_SENSOR_COUNT)
    {
        lastWriteSucceeded_ = false;
        return;
    }

    const unsigned long now = millis();

    if (now - lastLogAtMs_ < Config::LOG_INTERVAL_MS)
    {
        return;
    }

    // Advance the timestamp even when a write fails, avoiding
    // repeated writes on every loop iteration.
    lastLogAtMs_ = now;

    char timestamp[20];

    if (!clock_.getTimestamp(timestamp, sizeof(timestamp)))
    {
        lastWriteSucceeded_ = false;
        return;
    }

    char line[96];

    const int written = snprintf(
        line,
        sizeof(line),
        "%s,%.2f,%.2f,%.2f,%u",
        timestamp,
        readings[0].valid ? readings[0].celsius : -999.0f,
        readings[1].valid ? readings[1].celsius : -999.0f,
        readings[2].valid ? readings[2].celsius : -999.0f,
        (!readings[0].valid ||
         !readings[1].valid ||
         !readings[2].valid) ? 1U : 0U);

    if (written <= 0 ||
        static_cast<size_t>(written) >= sizeof(line))
    {
        lastWriteSucceeded_ = false;
        return;
    }

    lastWriteSucceeded_ =
        storage_.appendLine(LOG_FILENAME, line);
}

bool DataLogger::isReady() const
{
    return ready_;
}

bool DataLogger::lastWriteSucceeded() const
{
    return lastWriteSucceeded_;
}
} // namespace Services
} // namespace ColdGuard
