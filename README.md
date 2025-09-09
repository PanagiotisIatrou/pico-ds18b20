# pico-ds18b20
Raspberry Pi Pico library for the ds18b20 temperature sensor

## Run
usbipd list
cd \\wsl.localhost\Ubuntu-24.04\home\panagiotis\pico_projects\ds18b20
run bat from GUI

## Command codes
read rom: 0x33
match rom: 0x55
convert t: 0x44
skip rom: 0xCC
read scratchpad: 0xBE