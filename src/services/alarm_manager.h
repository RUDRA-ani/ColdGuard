
#ifndef COLDGUARD_ALARM_MANAGER_H
#define COLDGUARD_ALARM_MANAGER_H

#include <Arduino.h>

namespace ColdGuard
{
namespace Services
{
class AlarmManager
{
public:
    void begin();
    void setActive(bool active);
    bool isActive() const;

private:
    bool active_ = false;
};
} // namespace Services
} // namespace ColdGuard

#endif
