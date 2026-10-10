# ColdGuard — Embedded Cold-Storage Monitor

ColdGuard is an Arduino Nano-based embedded monitoring system designed to record temperature readings from multiple points in a refrigerated storage environment.

The project focuses on embedded C++, peripheral interfacing, timestamped data logging, fault handling, and maintainable firmware architecture.

## Planned Features

- Three DS18B20 temperature probes on a shared OneWire bus
- RTC-based timestamps using a DS1307 in simulation and a DS3231 in the intended hardware version
- CSV logging to a microSD card
- High- and low-temperature monitoring with hysteresis
- Sensor-disconnection detection
- Door-open monitoring
- Audible and visual alarms
- Recovery and error handling for storage and peripheral failures

## Hardware

- Arduino Nano (ATmega328P)
- Three DS18B20 temperature probes
- DS1307 / DS3231 RTC module
- MicroSD card module
- Buzzer and LED
- Door switch

## Software

- C++
- PlatformIO
- Arduino framework
- Wokwi simulation
- DallasTemperature and OneWire
- RTClib
- Arduino SD library

## Repository Structure

- `include/coldguard/` — shared configuration
- `src/drivers/` — hardware interfaces
- `src/services/` — monitoring, alarms, and logging
- `src/app/` — application initialization and coordination
- `test/` — automated tests
- `docs/` — architecture and verification documentation
- `hardware/` — schematic and PCB files

## Build

Install PlatformIO, then run:

```bash
pio run
```

Start the Wokwi simulation using the Wokwi extension and the configuration in `wokwi.toml`.

## Data Format

The intended CSV log format is:

```text
timestamp,temp1_c,temp2_c,temp3_c,sensor_fault
2026-10-10 12:00:00,4.25,4.50,5.00,0
```

Invalid sensor measurements are represented by `-999.0`; the `sensor_fault` field indicates whether any sensor reading was invalid.

## Development Status

Initial hardware interfaces and service modules are being developed. Full integration, fault-recovery testing, non-blocking sensor acquisition, and hardware validation remain in progress.

## Safety

ColdGuard is an educational monitoring prototype, not a certified food-safety or medical monitoring instrument. Its readings and alarms should not be relied upon as the sole means of protecting temperature-sensitive goods.