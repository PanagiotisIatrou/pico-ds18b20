#pragma once

enum OneWireState { READ, WRITE };

class OneWire {
private:
    int data_pin;
    OneWireState state;

public:
    OneWire(int data_pin);

    void set_state(OneWireState state);

    OneWireState get_state();

    void set_pin_value(bool value);

    void write_bit(bool value);

    bool read_bit();

    bool wait_for_bit(bool bit, int max_time_us);
};
