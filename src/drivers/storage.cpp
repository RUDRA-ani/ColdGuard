
#include "storage.h"

#include <SPI.h>
#include <SD.h>

#include "coldguard/config.h"

namespace ColdGuard
{
namespace Drivers
{
bool Storage::begin()
{
    pinMode(SS, OUTPUT);

    ready_ = SD.begin(Config::SD_CS_PIN);
    return ready_;
}

bool Storage::isReady() const
{
    return ready_;
}

bool Storage::appendLine(const char *filename, const char *line)
{
    if (!ready_ || filename == nullptr || line == nullptr)
    {
        return false;
    }

    File file = SD.open(filename, FILE_WRITE);

    if (!file)
    {
        return false;
    }

    const size_t expected = strlen(line);
    const size_t written = file.println(line);

    file.flush();
    file.close();

    // println() writes the text plus line ending.
    // A short write is treated as a logging failure.
    return written >= expected;
}
} // namespace Drivers
} // namespace ColdGuard
