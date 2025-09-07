#pragma once

#include "rom.hpp"
#include "one_wire.hpp"

class Device {
private:
    OneWire& one_wire;
    Rom rom;

    void send_command(const char command[8]);

public:

    Device(OneWire& one_wire);

    bool presence_pulse();

    void read_rom();

    void match_rom();
};
