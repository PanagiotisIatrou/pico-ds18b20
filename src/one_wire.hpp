#pragma once

#include <stdint.h>

enum OneWireState { READ, WRITE };

class OneWire {
private:
    int m_data_pin;
    OneWireState m_state;

public:
    OneWire(int data_pin);

    void set_state(OneWireState state);

    OneWireState get_state();

    bool get_pin_value();

    void set_pin_value(bool value);

    void write_bit(bool value);

    bool read_bit();

    void write_byte(uint8_t value);

    uint8_t read_byte();

    bool wait_us_for_bit(bool bit, int max_time_us);

    bool wait_ms_for_bit(bool bit, int max_time_ms);
};
