#include "device.hpp"

#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"

Device::Device(OneWire& one_wire) : one_wire(one_wire) { }

bool Device::presence_pulse() {
    // Write 0 to initialize connection
    one_wire.set_state(OneWireState::WRITE);
    one_wire.set_pin_value(0);
    sleep_us(500);

    // Wait for presence pulse
    one_wire.set_state(OneWireState::READ);
    sleep_us(5);
    bool detected_presence_pulse = one_wire.wait_us_for_bit(0, 240 + 55);
    if (!detected_presence_pulse) {
        return false;
    }

    // Wait for presence pulse to end
    bool detected_presence_pulse_end = one_wire.wait_us_for_bit(1, 240);
    if (!detected_presence_pulse_end) {
        return false;
    }

    return true;
}

void Device::skip_rom() {
    one_wire.write_byte(0xCC);
}

void Device::read_rom() {
    one_wire.write_byte(0x33);

    // Receive family code
    rom.family_code = one_wire.read_byte();

    // Receive family code
    for (int k = 5; k >= 0; k--) {
        rom.serial_number[k] = one_wire.read_byte();
    }

    // Receive CRC code
    rom.crc_code = one_wire.read_byte();
}

void Device::match_rom() {
    one_wire.write_byte(0x55);

    // Send family code
    one_wire.write_byte(rom.family_code);

    // Send serial number
    for (int k = 5; k >= 0; k--) {
        one_wire.write_byte(rom.serial_number[k]);
    }

    // Send CRC code
    one_wire.write_byte(rom.crc_code);
}

bool Device::convert_t() {
    one_wire.write_byte(0x44);

    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    while (to_ms_since_boot(get_absolute_time()) - start_time < 1000) {
        bool value = one_wire.read_bit();
        if (value) {
            return true;
        }
        sleep_ms(5);
    }

    return false;
}

void Device::read_scratchpad() {
    one_wire.write_byte(0xBE);

    for (int i = 0; i < 2; i++) {
        scratchpad.temperature[i] = one_wire.read_byte();
    }
    scratchpad.temperature_high = one_wire.read_byte();
    scratchpad.temperature_low = one_wire.read_byte();
    scratchpad.configuration = one_wire.read_byte();
    for (int i = 0; i < 3; i++) {
        scratchpad.reserved[i] = one_wire.read_byte();
    }
    scratchpad.crc_code = one_wire.read_byte();
}
