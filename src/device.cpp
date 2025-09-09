#include "device.hpp"

#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"

Device::Device(OneWire& one_wire) : m_one_wire(one_wire) { }

bool Device::presence_pulse() {
    // Write 0 to initialize connection
    m_one_wire.set_state(OneWireState::WRITE);
    m_one_wire.set_pin_value(0);
    sleep_us(500);

    // Wait for presence pulse
    m_one_wire.set_state(OneWireState::READ);
    sleep_us(5);
    bool detected_presence_pulse = m_one_wire.wait_us_for_bit(0, 240 + 55);
    if (!detected_presence_pulse) {
        return false;
    }

    // Wait for presence pulse to end
    bool detected_presence_pulse_end = m_one_wire.wait_us_for_bit(1, 240);
    if (!detected_presence_pulse_end) {
        return false;
    }

    return true;
}

void Device::skip_rom() {
    m_one_wire.write_byte(0xCC);
}

void Device::read_rom() {
    m_one_wire.write_byte(0x33);

    // Receive family code
    m_rom.family_code = m_one_wire.read_byte();

    // Receive family code
    for (int k = 5; k >= 0; k--) {
        m_rom.serial_number[k] = m_one_wire.read_byte();
    }

    // Receive CRC code
    m_rom.crc_code = m_one_wire.read_byte();
}

void Device::match_rom() {
    m_one_wire.write_byte(0x55);

    // Send family code
    m_one_wire.write_byte(m_rom.family_code);

    // Send serial number
    for (int k = 5; k >= 0; k--) {
        m_one_wire.write_byte(m_rom.serial_number[k]);
    }

    // Send CRC code
    m_one_wire.write_byte(m_rom.crc_code);
}

bool Device::convert_t() {
    m_one_wire.write_byte(0x44);

    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    while (to_ms_since_boot(get_absolute_time()) - start_time < 1000) {
        bool value = m_one_wire.read_bit();
        if (value) {
            return true;
        }
        sleep_ms(5);
    }

    return false;
}

void Device::read_scratchpad() {
    m_one_wire.write_byte(0xBE);

    for (int i = 0; i < 2; i++) {
        m_scratchpad.temperature[i] = m_one_wire.read_byte();
    }
    m_scratchpad.temperature_high = m_one_wire.read_byte();
    m_scratchpad.temperature_low = m_one_wire.read_byte();
    m_scratchpad.configuration = m_one_wire.read_byte();
    for (int i = 0; i < 3; i++) {
        m_scratchpad.reserved[i] = m_one_wire.read_byte();
    }
    m_scratchpad.crc_code = m_one_wire.read_byte();
}
