#pragma once

#include "device.hpp"

class Ds18b20 {
private:
    Device device;
    
    bool m_is_valid;

    const int m_max_tries = 10;

public:
    Ds18b20(OneWire& one_wire);

    bool ping();

    bool is_valid();

    float measure_temperature();

    uint8_t get_resolution();

    void set_resolution(Resolution resolution, bool save);
};
