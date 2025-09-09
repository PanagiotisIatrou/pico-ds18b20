#pragma once

#include "rom.hpp"
#include "scratchpad.hpp"
#include "one_wire.hpp"

class Device {
private:
    OneWire& one_wire;

    Rom rom;

    Scratchpad scratchpad;

public:

    Device(OneWire& one_wire);

    bool presence_pulse();

    void skip_rom();

    void read_rom();

    void match_rom();

    bool convert_t();

    void read_scratchpad();
};
