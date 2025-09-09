#pragma once

#include "rom.hpp"
#include "scratchpad.hpp"
#include "one_wire.hpp"

class Device {
private:
    OneWire& m_one_wire;

    Rom m_rom;

    Scratchpad m_scratchpad;

    uint8_t get_config_setting();

    float extract_temperature_from_scratchpad();

    bool presence_pulse();

    // ROM commands

    void skip_rom();

    void read_rom();

    void match_rom();

    // Function commands

    bool convert_t();

    void read_scratchpad();

public:
    Device(OneWire& one_wire);

    uint8_t get_resolution();

    float measure_temperature();
};
