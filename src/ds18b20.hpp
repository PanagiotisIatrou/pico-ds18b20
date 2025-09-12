#pragma once

#include "device.hpp"

class Ds18b20 {
private:
    Device device;
    
    bool m_is_valid;

    const int m_max_tries = 10;

    void set_scratchpad(bool save);

public:
    Ds18b20(OneWire& one_wire);

    bool ping();

    bool is_valid();

    float measure_temperature();

    uint8_t get_resolution();

    void set_resolution(Resolution resolution, bool save);

    int8_t get_temperature_low_limit();

    int8_t get_temperature_high_limit();

    void set_temperature_low_limit(int8_t temperature, bool save);

    void set_temperature_high_limit(int8_t temperature, bool save);
};
