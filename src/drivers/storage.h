
#ifndef COLDGUARD_STORAGE_H
#define COLDGUARD_STORAGE_H

#include <Arduino.h>

namespace ColdGuard
{
namespace Drivers
{
class Storage
{
public:
    bool begin();
    bool isReady() const;
    bool appendLine(const char *filename, const char *line);

private:
    bool ready_ = false;
};
} // namespace Drivers
} // namespace ColdGuard

#endif
