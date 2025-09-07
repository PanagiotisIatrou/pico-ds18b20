#include "rom.hpp"
#include "one_wire.hpp"

class Device {
private:
    OneWire& one_wire;

    void read_rom();

public:
    Rom rom;

    Device(OneWire& one_wire);

    bool presence_pulse();

    void send_rom_command(char command[8]);
};
