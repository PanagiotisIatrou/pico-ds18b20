#include "device.hpp"

#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"

Device::Device(OneWire& one_wire, Rom device_rom) : m_one_wire(one_wire) {
    rom = device_rom;
}

float Device::extract_temperature_from_scratchpad() {
    uint8_t config_setting = get_config_setting();
    int16_t temperature_data = scratchpad.temperature[0] + (scratchpad.temperature[1] << 8);
    temperature_data = temperature_data >> (3 - config_setting);
    float temperature = temperature_data / (float)(1 << (config_setting + 1));
    return temperature;
}

uint8_t Device::get_config_setting() {
    return (scratchpad.configuration & 0b01100000) >> 5;
}

uint8_t Device::resolution_to_configuration(Resolution resolution) {
    return 0b00011111 | ((uint8_t)resolution << 5);
}

void Device::skip_rom() {
    m_one_wire.write_byte(0xCC);
}

Device::ReadRomInfo Device::read_rom(OneWire& one_wire) {
    one_wire.write_byte(0x33);

    ReadRomInfo info{};

    // Receive family code
    info.rom.family_code = one_wire.read_byte();

    // Receive family code
    for (int k = 5; k >= 0; k--) {
        info.rom.serial_number[k] = one_wire.read_byte();
    }

    // Receive CRC code
    info.rom.crc_code = one_wire.read_byte();

    // Check ROM CRC
    uint8_t crc = 0;
    crc = OneWire::calculate_crc_byte(crc, info.rom.family_code);
    for (int i = 5; i >= 0; i--) {
        crc = OneWire::calculate_crc_byte(crc, info.rom.serial_number[i]);
    }
    info.is_valid = (crc != info.rom.crc_code && info.rom != Rom{});

    return info;
}

void Device::match_rom() {
    m_one_wire.write_byte(0x55);

    // Send family code
    m_one_wire.write_byte(rom.family_code);

    // Send serial number
    for (int k = 5; k >= 0; k--) {
        m_one_wire.write_byte(rom.serial_number[k]);
    }

    // Send CRC code
    m_one_wire.write_byte(rom.crc_code);
}

Device::SearchRomInfo Device::search_rom(OneWire& one_wire, uint64_t previous_sequence, int previous_sequence_length) {
    one_wire.write_byte(0xF0);

    SearchRomInfo info = {};
    uint64_t new_sequence = 0;
    for (int i = 0; i < 64; i++) {
        bool first_bit = one_wire.read_bit();
        bool second_bit = one_wire.read_bit();
        bool bit_choice;
        if (!first_bit && second_bit) {
            bit_choice = 0;
        } else if (first_bit && !second_bit) {
            bit_choice = 1;
        } else if (!first_bit && !second_bit){
            if (i < previous_sequence_length) {
                bit_choice = (previous_sequence >> i) & 0b1;
            } else if (i == previous_sequence_length) {
                bit_choice = 1;
            } else {
                bit_choice = 0;
                info.last_choice_path = new_sequence;
                info.last_choice_path_size = i;
            }
        } else {
            return {};
        }
        new_sequence |= ((uint64_t)bit_choice << i);
        one_wire.write_bit(bit_choice);
    }

    // Encode the rom
    Rom rom;
    rom.family_code = new_sequence & 0xFF;
    for (int i = 0; i < 6; i++) {
        rom.serial_number[5 - i] = (new_sequence >> (8 * (i + 1))) & 0xFF;
    }
    rom.crc_code = (new_sequence >> 56) & 0xFF;
    info.rom = rom;

    // Check rom CRC
    uint8_t crc = 0;
    crc = OneWire::calculate_crc_byte(crc, info.rom.family_code);
    for (int i = 5; i >= 0; i--) {
        crc = OneWire::calculate_crc_byte(crc, info.rom.serial_number[i]);
    }
    info.is_valid = (crc == info.rom.crc_code && info.rom != Rom{});

    return info;
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
        scratchpad.temperature[i] = m_one_wire.read_byte();
    }
    scratchpad.temperature_high_limit = m_one_wire.read_byte();
    scratchpad.temperature_low_limit = m_one_wire.read_byte();
    scratchpad.configuration = m_one_wire.read_byte();
    for (int i = 0; i < 3; i++) {
        scratchpad.reserved[i] = m_one_wire.read_byte();
    }
    scratchpad.crc_code = m_one_wire.read_byte();

    // Check Scratchpad CRC
    uint8_t crc = 0;
    for (int i = 0; i < 2; i++) {
        crc = OneWire::calculate_crc_byte(crc, scratchpad.temperature[i]);
    }
    crc = OneWire::calculate_crc_byte(crc, scratchpad.temperature_high_limit);
    crc = OneWire::calculate_crc_byte(crc, scratchpad.temperature_low_limit);
    crc = OneWire::calculate_crc_byte(crc, scratchpad.configuration);
    for (int i = 0; i < 3; i++) {
        crc = OneWire::calculate_crc_byte(crc, scratchpad.reserved[i]);
    }
    if (crc != scratchpad.crc_code) {
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

bool Device::read_power_supply() {
    m_one_wire.write_byte(0xB4);

    return m_one_wire.read_bit();
}
