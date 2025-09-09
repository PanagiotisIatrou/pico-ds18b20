# pico-ds18b20
Raspberry Pi Pico library for the ds18b20 temperature sensor

## Features
- Measure temperature
  - &plusmn;0.5°C from -10°C to +85°C
  - &plusmn;1°C from -30°C to +100°C
  - &plusmn;2°C from -55°C to +125°C
- Set resolution (temperature discrete step size)
  - Low: 0.0625°C steps
  - Medium: 0.125°C steps
  - High: 0.25°C steps
  - Very High: 0.5°C steps
- Save resolution for retention after power cycle
