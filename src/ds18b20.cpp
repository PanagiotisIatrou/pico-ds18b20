#include "ds18b20.hpp"

#include <stdio.h>

Ds18b20::Ds18b20(OneWire& one_wire, Rom rom) : m_one_wire(one_wire), device(one_wire, rom) {
    // Read the scratchpad
    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!m_one_wire.reset()) {
            continue;
        }
        device.match_rom();
        if (!device.read_scratchpad()) {
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

etl::vector<Ds18b20, 10> Ds18b20::search_rom(OneWire& one_wire) {
    etl::vector<Ds18b20, 10> devices;
    Device::SearchRomInfo info{};
    info.last_choice_path_size = -2;
    while (info.last_choice_path_size != -1) {
        // Reset
        bool ok = false;
        for (int t = 0; t < m_max_tries; t++) {
            if (!one_wire.reset()) {
                continue;
            }

            ok = true;
            break;
        }
        
        // Grab a ROM
        info = Device::search_rom(one_wire, info.last_choice_path, info.last_choice_path_size);
        if (info.is_valid) {
            devices.emplace_back(Ds18b20(one_wire, info.rom));
        }
    }

    printf("Found %d devices\n", devices.size());
    
    return devices;
}

bool Ds18b20::ping() {
    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        // Read the rom to see if it matches
        uint64_t rom = Rom::encode_rom(device.rom);
        if (!m_one_wire.reset()) {
            continue;
        }
        Device::SearchRomInfo info = Device::search_rom(m_one_wire, rom, 64);
        if (!info.is_valid || info.rom != device.rom) {
            continue;
        }

        // Check that power supply is external 
        if (!m_one_wire.reset()) {
            continue;
        }
        device.match_rom();
        if (!device.read_power_supply()) {
            continue;
        }

        ok = true;
        break;
    }

    m_is_valid = ok;
    return ok;
}

bool Ds18b20::is_valid() {
    return m_is_valid;
}

float Ds18b20::measure_temperature() {
    if (!m_is_valid) {
        return -1000.0;
    }

    // Request a temperature measurement
    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!m_one_wire.reset()) {
            continue;
        }
        device.match_rom();
        if (!device.convert_t()) {
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
        if (!m_one_wire.reset()) {
            continue;
        }
        device.match_rom();
        if (!device.read_scratchpad()) {
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
    return device.scratchpad.calculate_temperature();
}

uint8_t Ds18b20::get_resolution() {
    if (!m_is_valid) {
        return 0;
    }

    return device.scratchpad.get_resolution();
}

void Ds18b20::set_scratchpad(int8_t temperature_high_limit, int8_t temperature_low_limit, uint8_t configuration, bool save) {
    // Write the new values to the scratchpad and read it again
    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        // Write to the scratchpad
        if (!m_one_wire.reset()) {
            continue;
        }
        device.match_rom();
        device.write_scratchpad(temperature_high_limit, temperature_low_limit, configuration);

        // Read the scratchpad
        if (!m_one_wire.reset()) {
            continue;
        }
        device.match_rom();
        if (!device.read_scratchpad()) {
            continue;
        }
    
        ok = true;
        break;
    }
    if (!ok) {
        m_is_valid = false;
        return;
    }

    // Save the scratchpad if specified
    if (save) {
        bool ok = false;
        for (int t = 0; t < m_max_tries; t++) {
            if (!m_one_wire.reset()) {
                continue;
            }
            device.match_rom();
            if (!device.copy_scratchpad()) {
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

void Ds18b20::set_resolution(Resolution resolution, bool save) {
    if (!m_is_valid) {
        return;
    }
    uint8_t configuration = device.scratchpad.resolution_to_configuration(resolution);
}

int8_t Ds18b20::get_temperature_low_limit() {
    return device.scratchpad.get_temperature_low_limit();
}

int8_t Ds18b20::get_temperature_high_limit() {
    return device.scratchpad.get_temperature_high_limit();
}

void Ds18b20::set_temperature_low_limit(int8_t temperature, bool save) {
    if (!m_is_valid) {
        return;
    }

    set_scratchpad(device.scratchpad.get_temperature_high_limit(), temperature, device.scratchpad.get_configuration(), save);
}

void Ds18b20::set_temperature_high_limit(int8_t temperature, bool save) {
    if (!m_is_valid) {
        return;
    }

    set_scratchpad(temperature, device.scratchpad.get_temperature_low_limit(), device.scratchpad.get_configuration(), save);
}
