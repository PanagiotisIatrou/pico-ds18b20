#include "ds18b20.hpp"

Ds18b20::Ds18b20(OneWire& one_wire) : device(one_wire) {
    // Read the ROM of the device
    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!device.presence_pulse()) {
            continue;
        }
        if (!device.read_rom()) {
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
        if (!device.presence_pulse()) {
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

bool Ds18b20::ping() {
    Rom old_rom = device.rom;

    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!device.presence_pulse()) {
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

uint8_t Ds18b20::get_resolution() {
    if (!m_is_valid) {
        return 0;
    }

    return 9 + device.get_config_setting();
}

float Ds18b20::measure_temperature() {
    if (!m_is_valid) {
        return -1000.0;
    }

    // Request a temperature measurement
    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!device.presence_pulse()) {
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
        if (!device.presence_pulse()) {
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

void Ds18b20::set_resolution(Resolution resolution, bool save) {
    if (!m_is_valid) {
        return;
    }

    // Write the new resolution to the scratchpad
    bool ok = false;
    for (int t = 0; t < m_max_tries; t++) {
        if (!device.presence_pulse()) {
            continue;
        }
        device.match_rom();
        device.scratchpad.configuration = device.resolution_to_configuration(resolution);
        device.write_scratchpad(device.scratchpad.temperature_high, device.scratchpad.temperature_low, device.scratchpad.configuration);
    
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
            if (!device.presence_pulse()) {
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
