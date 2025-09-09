#pragma once

#include "rom.hpp"
#include "scratchpad.hpp"
#include "one_wire.hpp"

enum class Resolution { Low = 0b00, Medium = 0b01, High = 0b10, VeryHigh = 0b11 };

class Device {
private:
    OneWire& m_one_wire;

    Rom m_rom;

    Scratchpad m_scratchpad;

    bool m_is_valid;

    const int m_max_tries = 10;

    uint8_t get_config_setting();

    float extract_temperature_from_scratchpad();

    bool presence_pulse();

    // ROM commands

    void skip_rom();

    bool read_rom();

    void match_rom();

    // Function commands

    bool convert_t();

    bool read_scratchpad();

    void write_scratchpad(int8_t temperature_high, int8_t temperature_low, uint8_t configuration);

    bool copy_scratchpad();

public:
    Device(OneWire& one_wire);

    bool ping();

    bool is_valid();

    uint8_t get_resolution();

    float measure_temperature();

    void set_resolution(Resolution resolution, bool save);
};
