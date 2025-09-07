#include "one_wire.hpp"

#include "pico/stdlib.h"

void initialize_one_wire(OneWire& one_wire, int data_pin) {
    one_wire.data_pin = data_pin;
    gpio_init(data_pin);
    gpio_pull_up(data_pin);
}

void set_state(OneWire& one_wire, OneWireState state) {
    one_wire.state = state;
    bool gpio_state = state == OneWireState::READ ? GPIO_IN : GPIO_OUT;
    gpio_set_dir(one_wire.data_pin, gpio_state);
}

OneWireState get_state(OneWire& one_wire) {
    return one_wire.state;
}

void set_pin_value(OneWire& one_wire, bool value) {
    gpio_put(one_wire.data_pin, value);
}

void write_bit(OneWire& one_wire, bool value) {
    gpio_set_dir(one_wire.data_pin, GPIO_OUT);
    gpio_put(one_wire.data_pin, 0);
    if (value) {
        sleep_us(8);
        gpio_set_dir(one_wire.data_pin, GPIO_IN);
        sleep_us(52);
    } else {
        sleep_us(60);
        gpio_set_dir(one_wire.data_pin, GPIO_IN);
    }
}

bool read_bit(OneWire& one_wire) {
    gpio_set_dir(one_wire.data_pin, GPIO_OUT);
    gpio_put(one_wire.data_pin, 0);
    sleep_us(5);
    gpio_set_dir(one_wire.data_pin, GPIO_IN);
    sleep_us(10);
    bool data = gpio_get(one_wire.data_pin);
    sleep_us(45);

    return data;
}

bool wait_for_bit(OneWire& one_wire, bool bit, int max_time_us) {
    uint32_t start_time = to_us_since_boot(get_absolute_time());
    while (to_us_since_boot(get_absolute_time()) - start_time < max_time_us) {
        if (gpio_get(one_wire.data_pin) == bit) {
            return true;
        }

        sleep_us(1);
    }

    return false;
}
