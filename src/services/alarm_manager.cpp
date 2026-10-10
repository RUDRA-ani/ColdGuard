 #include "alarm_manager.h"

#include "coldguard/config.h"

namespace ColdGuard
{
namespace Services
{
void AlarmManager::begin()
{
    pinMode(Config::BUZZER_PIN, OUTPUT);
    pinMode(Config::ALARM_LED_PIN, OUTPUT);

    active_ = false;

    noTone(Config::BUZZER_PIN);
    digitalWrite(Config::BUZZER_PIN, LOW);
    digitalWrite(Config::ALARM_LED_PIN, LOW);
     active_ = false;
}

void AlarmManager::setActive(bool active)
{
    // Avoid repeatedly restarting the buzzer.
    if (active == active_)
    {
        return;
    }

    active_ = active;

    digitalWrite(
        Config::ALARM_LED_PIN,
        active_ ? HIGH : LOW);

    if (active_)
    {
        tone(Config::BUZZER_PIN, 2000);
    }
    else
    {
        noTone(Config::BUZZER_PIN);
        digitalWrite(Config::BUZZER_PIN, LOW);
    }
}

bool AlarmManager::isActive() const
{
    return active_;
}
} // namespace Services
} // namespace ColdGuard
 