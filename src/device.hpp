#pragma once

#include "rom.hpp"
#include "scratchpad.hpp"
#include "one_wire.hpp"

enum class Resolution { Low = 0b00, Medium = 0b01, High = 0b10, VeryHigh = 0b11 };

class Device {
private:
    OneWire& m_one_wire;

public:
    Device(OneWire& one_wire);

    Rom rom;

    Scratchpad scratchpad;

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

    // Helper functions

    float extract_temperature_from_scratchpad();

    uint8_t get_config_setting();

    uint8_t resolution_to_configuration(Resolution resolution);
};
