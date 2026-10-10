
#include "door_sensor.h"

#include "coldguard/config.h"

namespace ColdGuard
{
namespace Drivers
{
void DoorSensor::begin()
{
    pinMode(Config::DOOR_SENSOR_PIN, INPUT_PULLUP);

    open_ = digitalRead(Config::DOOR_SENSOR_PIN) == LOW;
    candidateOpen_ = open_;

    const unsigned long now = millis();
    candidateSinceMs_ = now;
    openedAtMs_ = open_ ? now : 0;
}

void DoorSensor::update()
{
    const unsigned long now = millis();
    const bool rawOpen =
        digitalRead(Config::DOOR_SENSOR_PIN) == LOW;

    if (rawOpen != candidateOpen_)
    {
        candidateOpen_ = rawOpen;
        candidateSinceMs_ = now;
    }

    if (candidateOpen_ != open_ &&
        now - candidateSinceMs_ >= DEBOUNCE_MS)
    {
        open_ = candidateOpen_;

        if (open_)
        {
            openedAtMs_ = now;
        }
        else
        {
            openedAtMs_ = 0;
        }
    }
}

bool DoorSensor::isOpen() const
{
    return open_;
}

bool DoorSensor::hasBeenOpenFor(unsigned long durationMs) const
{
    return open_ && (millis() - openedAtMs_ >= durationMs);
}
} // namespace Drivers
} // namespace ColdGuard
