#include "device_commands.hpp"

#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"

#include "common.hpp"

void DeviceCommands::skip_rom(const OneWire& one_wire) {
    uint8_t command = static_cast<uint8_t>(RomCommands::SkipRom);
    one_wire.write_byte(command);
}

std::optional<Rom> DeviceCommands::read_rom(const OneWire& one_wire) {
    uint8_t command = static_cast<uint8_t>(RomCommands::ReadRom);
    one_wire.write_byte(command);

    // Read the rom
    uint8_t family_code = one_wire.read_byte();
    uint8_t serial_number[6];
    for (int i = 0; i < 6; i++) {
        serial_number[i] = one_wire.read_byte();
    }
    uint8_t crc_code = one_wire.read_byte();
    Rom rom(family_code, serial_number, crc_code);

    // Return the rom
    if (!rom.is_empty() && rom.has_valid_crc()) {
        return rom;
    } else {
        return std::nullopt;
    }
}

void DeviceCommands::match_rom(const OneWire& one_wire, const Rom& rom) {
    uint8_t command = static_cast<uint8_t>(RomCommands::MatchRom);
    one_wire.write_byte(command);

    // Send family code
    one_wire.write_byte(rom.get_family_code());

    // Send serial number
    for (int i = 0; i < 6; i++) {
        one_wire.write_byte(rom.get_serial_number(i));
    }

    // Send CRC code
    one_wire.write_byte(rom.get_crc_code());
}

std::optional<DeviceCommands::SearchInfo> DeviceCommands::search(const OneWire& one_wire, uint64_t previous_sequence, int previous_sequence_length) {
    SearchInfo info = {};
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
            return std::nullopt;
        }
        new_sequence |= ((uint64_t)bit_choice << i);
        one_wire.write_bit(bit_choice);
    }

    info.rom = Rom::decode_rom(new_sequence);
    if (!info.rom.is_empty() && info.rom.has_valid_crc()) {
        return info;
    } else {
        return std::nullopt;
    }
}

std::optional<DeviceCommands::SearchInfo> DeviceCommands::search_rom(const OneWire& one_wire, uint64_t previous_sequence, int previous_sequence_length) {
    uint8_t command = static_cast<uint8_t>(RomCommands::SearchRom);
    one_wire.write_byte(command);

    return search(one_wire, previous_sequence, previous_sequence_length);
}

std::optional<DeviceCommands::SearchInfo> DeviceCommands::search_alarm(const OneWire& one_wire, uint64_t previous_sequence, int previous_sequence_length) {
    uint8_t command = static_cast<uint8_t>(RomCommands::SearchAlarm);
    one_wire.write_byte(command);

    return search(one_wire, previous_sequence, previous_sequence_length);
}

std::optional<uint32_t> DeviceCommands::convert_t(const OneWire& one_wire) {
    uint8_t command = static_cast<uint8_t>(FunctionCommands::ConvertT);
    one_wire.write_byte(command);

    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    while (to_ms_since_boot(get_absolute_time()) - start_time < 1000) {
        bool value = one_wire.read_bit();
        if (value) {
            return to_ms_since_boot(get_absolute_time()) - start_time;
        }
        sleep_ms(5);
    }

    return std::nullopt;
}

std::optional<Scratchpad> DeviceCommands::read_scratchpad(const OneWire& one_wire) {
    uint8_t command = static_cast<uint8_t>(FunctionCommands::ReadScratchpad);
    one_wire.write_byte(command);

    // Read the scratchpad
    uint8_t temperature[2];
    for (int i = 0; i < 2; i++) {
        temperature[i] = one_wire.read_byte();
    }
    uint8_t temperature_high_limit = one_wire.read_byte();
    uint8_t temperature_low_limit = one_wire.read_byte();
    uint8_t configuration = one_wire.read_byte();
    uint8_t reserved[3];
    for (int i = 0; i < 3; i++) {
        reserved[i] = one_wire.read_byte();
    }
    uint8_t crc_code = one_wire.read_byte();
    Scratchpad scratchpad = Scratchpad(temperature, temperature_high_limit, temperature_low_limit, configuration, reserved, crc_code);

    // Check Scratchpad CRC
    if (scratchpad.has_valid_crc()) {
        return scratchpad;
    } else {
        return std::nullopt;
    }
}

void DeviceCommands::write_scratchpad(const OneWire& one_wire, int8_t temperature_high, int8_t temperature_low, uint8_t configuration) {
    uint8_t command = static_cast<uint8_t>(FunctionCommands::WriteScratchpad);
    one_wire.write_byte(command);

    one_wire.write_byte(temperature_high);
    one_wire.write_byte(temperature_low);
    one_wire.write_byte(configuration);
}

std::optional<uint32_t> DeviceCommands::copy_scratchpad(const OneWire& one_wire) {
    uint8_t command = static_cast<uint8_t>(FunctionCommands::CopyScratchpad);
    one_wire.write_byte(command);

    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    while (to_ms_since_boot(get_absolute_time()) - start_time < 1000) {
        bool value = one_wire.read_bit();
        if (value) {
            return to_ms_since_boot(get_absolute_time()) - start_time;
        }
        sleep_ms(1);
    }

    return std::nullopt;
}

PowerSupplyMode DeviceCommands::read_power_supply_mode(const OneWire& one_wire) {
    uint8_t command = static_cast<uint8_t>(FunctionCommands::ReadPowerSupply);
    one_wire.write_byte(command);

    bool bit = one_wire.read_bit();
    if (bit) {
        return PowerSupplyMode::External;
    } else {
        return PowerSupplyMode::Parasite;
    }
}
