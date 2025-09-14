#pragma once

#include <optional>

#include "rom.hpp"
#include "scratchpad.hpp"
#include "one_wire.hpp"

class Device {
protected:
    struct SearchInfo {
        Rom rom;
        uint64_t last_choice_path = 0;
        int last_choice_path_size = -1;
    };
    
private:
    OneWire& m_one_wire;

    static std::optional<Device::SearchInfo> search(OneWire& one_wire, uint64_t previous_sequence, int previous_sequence_length);

protected:
    Device(OneWire& one_wire, Rom rom);

    Rom m_rom;

    Scratchpad m_scratchpad;

    // ROM commands

    void skip_rom();

    static std::optional<Rom> read_rom(OneWire& one_wire);

    void match_rom();

    static std::optional<Device::SearchInfo> search_rom(OneWire& one_wire, uint64_t previous_sequence, int previous_sequence_length);

    static std::optional<Device::SearchInfo> search_alarm(OneWire& one_wire, uint64_t previous_sequence, int previous_sequence_length);

    // Function commands

    std::optional<uint32_t> convert_t();

    bool read_scratchpad();

    void write_scratchpad(int8_t temperature_high, int8_t temperature_low, uint8_t configuration);

    std::optional<uint32_t> copy_scratchpad();

    bool read_power_supply();
};
