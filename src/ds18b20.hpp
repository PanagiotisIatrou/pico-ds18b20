#pragma once

#include "device.hpp"

#include "etl/vector.h"

class Ds18b20 {
private:
    Device device;

    OneWire& m_one_wire;
    
    bool m_is_valid;

    static const int m_max_tries = 10;

    bool set_scratchpad(int8_t temperature_high_limit, int8_t temperature_low_limit, uint8_t configuration, bool save);

public:
    Ds18b20(OneWire& one_wire, Rom rom);

    static etl::vector<Ds18b20, 10> search_rom(OneWire& one_wire);

    bool ping();

    bool is_valid();

    std::optional<float> measure_temperature();

    uint8_t get_resolution();

    bool set_resolution(Resolution resolution, bool save);

    int8_t get_temperature_low_limit();

    int8_t get_temperature_high_limit();

    bool set_temperature_low_limit(int8_t temperature, bool save);

    bool set_temperature_high_limit(int8_t temperature, bool save);
};
