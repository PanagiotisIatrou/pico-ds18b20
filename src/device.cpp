#include "device.hpp"

#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"

Device::Device(OneWire& one_wire, Rom device_rom) : m_one_wire(one_wire) {
    rom = device_rom;
}

void Device::skip_rom() {
    m_one_wire.write_byte(0xCC);
}

Device::ReadRomInfo Device::read_rom(OneWire& one_wire) {
    one_wire.write_byte(0x33);

    // Read the rom
    uint8_t family_code = one_wire.read_byte();
    uint8_t serial_number[6];
    for (int i = 0; i < 6; i++) {
        serial_number[i] = one_wire.read_byte();
    }
    uint8_t crc_code = one_wire.read_byte();

    // Create the return object
    Rom rom(family_code, serial_number, crc_code);
    bool is_valid = (!rom.is_empty() && rom.has_valid_crc());
    ReadRomInfo info =  {.rom = rom, .is_valid = is_valid};

    return info;
}

void Device::match_rom() {
    m_one_wire.write_byte(0x55);

    // Send family code
    m_one_wire.write_byte(rom.get_family_code());

    // Send serial number
    for (int i = 0; i < 6; i++) {
        m_one_wire.write_byte(rom.get_serial_number(i));
    }

    // Send CRC code
    m_one_wire.write_byte(rom.get_crc_code());
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

    info.rom = Rom::decode_rom(new_sequence);

    // Check rom CRC
    info.is_valid = (!info.rom.is_empty() && info.rom.has_valid_crc());

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
    uint8_t temperature[2];
    for (int i = 0; i < 2; i++) {
        temperature[i] = m_one_wire.read_byte();
    }
    uint8_t temperature_high_limit = m_one_wire.read_byte();
    uint8_t temperature_low_limit = m_one_wire.read_byte();
    uint8_t configuration = m_one_wire.read_byte();
    uint8_t reserved[3];
    for (int i = 0; i < 3; i++) {
        reserved[i] = m_one_wire.read_byte();
    }
    uint8_t crc_code = m_one_wire.read_byte();
    scratchpad = Scratchpad(temperature, temperature_high_limit, temperature_low_limit, configuration, reserved, crc_code);

    // Check Scratchpad CRC
    if (!scratchpad.has_valid_crc()) {
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
