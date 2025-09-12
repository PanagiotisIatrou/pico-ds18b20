#include "ds18b20.hpp"

#include <stdio.h>

Ds18b20::Ds18b20(OneWire& one_wire, Rom rom) : m_one_wire(one_wire), device(one_wire, rom) {
    device.rom = rom;

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

    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!one_wire.reset()) {
            continue;
        }

        ok = true;
        break;
    }
    Rom rom = Device::search_rom(one_wire, 0, 0, false, 0);
    devices.emplace_back(Ds18b20(one_wire, rom));
    return devices;
}

bool Ds18b20::ping() {
    Rom old_rom = device.rom;

    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!m_one_wire.reset()) {
            continue;
        }
        if (!device.read_rom()) {
            continue;
        }

        ok = true;
        break;
    }

    if (ok && device.rom == old_rom) {
        m_is_valid = true;
        return true;
    }

    device.rom = old_rom;
    m_is_valid = false;
    return false;
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
    return device.extract_temperature_from_scratchpad();
}

uint8_t Ds18b20::get_resolution() {
    if (!m_is_valid) {
        return 0;
    }

    return 9 + device.get_config_setting();
}

void Ds18b20::set_scratchpad(bool save) {
    // Write the new resolution to the scratchpad
    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!m_one_wire.reset()) {
            continue;
        }
        device.match_rom();
        device.write_scratchpad(device.scratchpad.temperature_high_limit, device.scratchpad.temperature_low_limit, device.scratchpad.configuration);
    
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
    uint8_t configuration = device.resolution_to_configuration(resolution);
}

int8_t Ds18b20::get_temperature_low_limit() {
    return device.scratchpad.temperature_low_limit;
}

int8_t Ds18b20::get_temperature_high_limit() {
    return device.scratchpad.temperature_high_limit;
}

void Ds18b20::set_temperature_low_limit(int8_t temperature, bool save) {
    if (!m_is_valid) {
        return;
    }

    device.scratchpad.temperature_low_limit = temperature;
    set_scratchpad(save);
}

void Ds18b20::set_temperature_high_limit(int8_t temperature, bool save) {
    if (!m_is_valid) {
        return;
    }

    device.scratchpad.temperature_high_limit = temperature;
    set_scratchpad(save);
}
