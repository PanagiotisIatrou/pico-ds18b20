#include "device.hpp"

#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"

Device::Device(OneWire& one_wire) : m_one_wire(one_wire) {
    // Read the ROM of the device
    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!presence_pulse()) {
            continue;
        }
        if (!read_rom()) {
            continue;
        }

        ok = true;
        break;
    }
    if (!ok) {
        m_is_valid = false;
        return;
    }

    // Read the scratchpad
    ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!presence_pulse()) {
            continue;
        }
        match_rom();
        if (!read_scratchpad()) {
            continue;
        }

        ok = true;
        break;
    }
    if (!ok) {
        m_is_valid = false;
        return;
    }

    m_is_valid = true;
}

uint8_t Device::get_config_setting() {
    return (m_scratchpad.configuration & 0b01100000) >> 5;
}

bool Device::is_valid() {
    return m_is_valid;
}

uint8_t Device::get_resolution() {
    if (!m_is_valid) {
        return 0;
    }

    return 9 + get_config_setting();
}

float Device::extract_temperature_from_scratchpad() {
    uint8_t config_setting = get_config_setting();
    int16_t temperature_data = m_scratchpad.temperature[0] + (m_scratchpad.temperature[1] << 8);
    temperature_data = temperature_data >> (3 - config_setting);
    float temperature = temperature_data / (float)(1 << (config_setting + 1));
    return temperature;
}

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

bool Device::read_rom() {
    m_one_wire.write_byte(0x33);

    // Receive family code
    m_rom.family_code = m_one_wire.read_byte();

    // Receive family code
    for (int k = 5; k >= 0; k--) {
        m_rom.serial_number[k] = m_one_wire.read_byte();
    }

    // Receive CRC code
    m_rom.crc_code = m_one_wire.read_byte();

    // Check ROM CRC
    uint8_t crc = 0;
    crc = OneWire::calculate_crc_byte(crc, m_rom.family_code);
    for (int i = 5; i >= 0; i--) {
        crc = OneWire::calculate_crc_byte(crc, m_rom.serial_number[i]);
    }
    if (crc != m_rom.crc_code) {
        return false;
    }

    return true;
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

bool Device::read_scratchpad() {
    m_one_wire.write_byte(0xBE);

    // Read the scratchpad
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

    // Check Scratchpad CRC
    uint8_t crc = 0;
    for (int i = 0; i < 2; i++) {
        crc = OneWire::calculate_crc_byte(crc, m_scratchpad.temperature[i]);
    }
    crc = OneWire::calculate_crc_byte(crc, m_scratchpad.temperature_high);
    crc = OneWire::calculate_crc_byte(crc, m_scratchpad.temperature_low);
    crc = OneWire::calculate_crc_byte(crc, m_scratchpad.configuration);
    for (int i = 0; i < 3; i++) {
        crc = OneWire::calculate_crc_byte(crc, m_scratchpad.reserved[i]);
    }
    if (crc != m_scratchpad.crc_code) {
        return false;
    }

    return true;
}

void Device::write_scratchpad(int8_t temperature_high, int8_t temperature_low, uint8_t configuration) {
    m_one_wire.write_byte(0x4E);

    m_one_wire.write_byte(temperature_high);
    m_one_wire.write_byte(temperature_low);
    m_one_wire.write_byte(configuration);
}

bool Device::copy_scratchpad() {
    m_one_wire.write_byte(0x48);

    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    while (to_ms_since_boot(get_absolute_time()) - start_time < 1000) {
        bool value = m_one_wire.read_bit();
        if (value) {
            return true;
        }
        sleep_ms(1);
    }

    return false;
}

float Device::measure_temperature() {
    if (!m_is_valid) {
        return -1000.0;
    }

    // Request a temperature measurement
    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!presence_pulse()) {
            continue;
        }
        match_rom();
        if (!convert_t()) {
            continue;
        }

        ok = true;
        break;
    }
    if (!ok) {
        m_is_valid = false;
        return -1000.0;
    }

    // Read the scratchpad
    ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!presence_pulse()) {
            continue;
        }
        match_rom();
        if (!read_scratchpad()) {
            continue;
        }

        ok = true;
        break;
    }
    if (!ok) {
        m_is_valid = false;
        return -1000.0;
    }

    // Extract the temperature from the scratchpad
    return extract_temperature_from_scratchpad();
}

void Device::set_resolution(Resolution resolution, bool save) {
    if (!m_is_valid) {
        return;
    }

    // Write the new resolution to the scratchpad
    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!presence_pulse()) {
            continue;
        }
        match_rom();
        m_scratchpad.configuration = 0b00011111 | ((uint8_t)resolution << 5);
        write_scratchpad(m_scratchpad.temperature_high, m_scratchpad.temperature_low, m_scratchpad.configuration);
    
        ok = true;
        break;
    }
    if (!ok) {
        m_is_valid = false;
        return;
    }

    // Save the resolution if specified
    if (save) {
        bool ok = false;
        for (int t = 0; t < m_max_tries; t++) {
            if (!presence_pulse()) {
                continue;
            }
            match_rom();
            if (!copy_scratchpad()) {
                continue;
            }

            ok = true;
            break;
        }
        if (!ok) {
            m_is_valid = false;
            return;
        }
    }
}
