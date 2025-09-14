#include "ds18b20.hpp"

#include <stdio.h>

Ds18b20::Ds18b20(OneWire& one_wire, Rom rom) : m_one_wire(one_wire), Device(one_wire, rom) {
    // Read the scratchpad
    for (int t = 0; t < m_max_tries; t++) {
        if (!m_one_wire.reset()) {
            continue;
        }
        match_rom();
        if (!read_scratchpad()) {
            continue;
        }

        is_initialized = true;
        return;
    }

    is_initialized = false;
}

bool Ds18b20::is_successfully_initialized() {
    return is_initialized;
}

etl::vector<Ds18b20, 10> Ds18b20::find_devices(OneWire& one_wire) {
    etl::vector<Ds18b20, 10> devices;
    Device::SearchInfo info{};
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
        auto result = Device::search_rom(one_wire, info.last_choice_path, info.last_choice_path_size);
        if (result.has_value()) {
            info = result.value();
            Ds18b20 device(one_wire, info.rom);
            if (device.is_successfully_initialized()) {
                devices.emplace_back(device);
            }
        }
    }

    printf("Found %d devices\n", devices.size());
    
    return devices;
}

bool Ds18b20::ping() {
    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        // Read the rom to see if it matches
        uint64_t rom = Rom::encode_rom(m_rom);
        if (!m_one_wire.reset()) {
            continue;
        }
        auto result = Device::search_rom(m_one_wire, rom, 64);
        if (result.has_value()) {
            Device::SearchInfo info = result.value();
            if (info.rom != m_rom) {
                continue;
            }
        } else {
            continue;
        }

        // Check that power supply is external 
        if (!m_one_wire.reset()) {
            continue;
        }
        match_rom();
        if (!read_power_supply()) {
            continue;
        }

        ok = true;
        break;
    }

    return ok;
}

std::optional<float> Ds18b20::measure_temperature() {
    // Request a temperature measurement
    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!m_one_wire.reset()) {
            continue;
        }
        match_rom();
        if (!convert_t().has_value()) {
            continue;
        }

        ok = true;
        break;
    }
    if (!ok) {
        return std::nullopt;
    }

    // Read the scratchpad
    ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!m_one_wire.reset()) {
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
        return std::nullopt;
    }

    // Extract the temperature from the scratchpad
    return m_scratchpad.calculate_temperature();
}

uint8_t Ds18b20::get_resolution() {

    return m_scratchpad.get_resolution();
}

bool Ds18b20::set_scratchpad(int8_t temperature_high_limit, int8_t temperature_low_limit, uint8_t configuration, bool save) {
    // Write the new values to the scratchpad and read it again
    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        // Write to the scratchpad
        if (!m_one_wire.reset()) {
            continue;
        }
        match_rom();
        write_scratchpad(temperature_high_limit, temperature_low_limit, configuration);

        // Read the scratchpad
        if (!m_one_wire.reset()) {
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
        return false;
    }

    // Save the scratchpad if specified
    if (save) {
        bool ok = false;
        for (int t = 0; t < m_max_tries; t++) {
            if (!m_one_wire.reset()) {
                continue;
            }
            match_rom();
            if (!copy_scratchpad().has_value()) {
                continue;
            }

            ok = true;
            break;
        }
        if (!ok) {
            return false;
        }
    }

    return true;
}

bool Ds18b20::set_resolution(Resolution resolution, bool save) {
    uint8_t configuration = m_scratchpad.resolution_to_configuration(resolution);
    return set_scratchpad(m_scratchpad.get_temperature_high_limit(), m_scratchpad.get_temperature_low_limit(), configuration, save);
}

int8_t Ds18b20::get_temperature_low_limit() {
    return m_scratchpad.get_temperature_low_limit();
}

int8_t Ds18b20::get_temperature_high_limit() {
    return m_scratchpad.get_temperature_high_limit();
}

bool Ds18b20::set_temperature_low_limit(int8_t temperature, bool save) {
    return set_scratchpad(m_scratchpad.get_temperature_high_limit(), temperature, m_scratchpad.get_configuration(), save);
}

bool Ds18b20::set_temperature_high_limit(int8_t temperature, bool save) {
    return set_scratchpad(temperature, m_scratchpad.get_temperature_low_limit(), m_scratchpad.get_configuration(), save);
}

bool Ds18b20::is_alarm_active() {
    for (int t = 0; t < m_max_tries; t++) {
        uint64_t rom = Rom::encode_rom(m_rom);
        if (!m_one_wire.reset()) {
            continue;
        }
        auto result = Device::search_alarm(m_one_wire, rom, 64);
        if (result.has_value()) {
            Device::SearchInfo info = result.value();
            if (info.rom == m_rom) {
                return true;
            } else {
                return false;
            }
        } else {
            continue;
        }
    }

    return false;
}
