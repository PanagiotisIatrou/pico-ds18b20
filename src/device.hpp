#pragma once

#include "rom.hpp"
#include "scratchpad.hpp"
#include "one_wire.hpp"

enum class Resolution { Low = 0b00, Medium = 0b01, High = 0b10, VeryHigh = 0b11 };

class Device {
private:
    OneWire& m_one_wire;

public:
    Device(OneWire& one_wire, Rom device_rom);

    Rom rom;

    Scratchpad scratchpad;

    // ROM commands

    void skip_rom();

    bool read_rom();

    void match_rom();

    struct SearchRomInfo {
        Rom rom;
        uint64_t last_choice_path = 0;
        int last_choice_path_size = -1;
        bool is_crc_valid = false;
    };

    static SearchRomInfo search_rom(OneWire& one_wire, uint64_t previous_sequence, int previous_sequence_length);

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
