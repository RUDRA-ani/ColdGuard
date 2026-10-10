
#ifndef COLDGUARD_DOOR_SENSOR_H
#define COLDGUARD_DOOR_SENSOR_H

#include <Arduino.h>

namespace ColdGuard
{
namespace Drivers
{
class DoorSensor
{
public:
    void begin();
    void update();

    bool isOpen() const;
    bool hasBeenOpenFor(unsigned long durationMs) const;

private:
    bool open_ = false;
    unsigned long openedAtMs_ = 0;

    bool candidateOpen_ = false;
    unsigned long candidateSinceMs_ = 0;

    static const unsigned long DEBOUNCE_MS = 40;
};
} // namespace Drivers
} // namespace ColdGuard

#endif
