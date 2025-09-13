#pragma once

#include "rom.hpp"
#include "scratchpad.hpp"
#include "one_wire.hpp"

class Device {
private:
    OneWire& m_one_wire;

public:
    Device(OneWire& one_wire, Rom device_rom);

    Rom rom;

    Scratchpad scratchpad;

    // ROM commands

    void skip_rom();

    struct ReadRomInfo {
        Rom rom;
        bool is_valid = true;
    };

    static ReadRomInfo read_rom(OneWire& one_wire);

    void match_rom();

    struct SearchRomInfo {
        Rom rom;
        uint64_t last_choice_path = 0;
        int last_choice_path_size = -1;
        bool is_valid = false;
    };

    static SearchRomInfo search_rom(OneWire& one_wire, uint64_t previous_sequence, int previous_sequence_length);

    // Function commands

    bool convert_t();

    bool read_scratchpad();

    void write_scratchpad(int8_t temperature_high, int8_t temperature_low, uint8_t configuration);

    bool copy_scratchpad();

    bool read_power_supply();
};
