#pragma once

#include "rom.hpp"
#include "scratchpad.hpp"
#include "one_wire.hpp"

class Device {
private:
    OneWire& m_one_wire;

    Rom m_rom;

    Scratchpad m_scratchpad;

    bool presence_pulse();

    void skip_rom();

    void read_rom();

    void match_rom();

    bool convert_t();

    void read_scratchpad();

public:
    Device(OneWire& one_wire);

    float measure_temperature();
};
