#include "one_wire.hpp"

#include "pico/stdlib.h"

OneWire::OneWire(int data_pin) : data_pin(data_pin) {
    gpio_init(data_pin);
    gpio_pull_up(data_pin);
}

void OneWire::set_state(OneWireState state) {
    state = state;
    bool gpio_state = state == OneWireState::READ ? GPIO_IN : GPIO_OUT;
    gpio_set_dir(data_pin, gpio_state);
}

OneWireState OneWire::get_state() {
    return state;
}

void OneWire::get_pin_value() {
    gpio_get(data_pin);
}

void OneWire::set_pin_value(bool value) {
    gpio_put(data_pin, value);
}

void OneWire::write_bit(bool value) {
    gpio_set_dir(data_pin, GPIO_OUT);
    gpio_put(data_pin, 0);
    if (value) {
        sleep_us(8);
        gpio_set_dir(data_pin, GPIO_IN);
        sleep_us(52);
    } else {
        sleep_us(60);
        gpio_set_dir(data_pin, GPIO_IN);
    }
}

bool OneWire::read_bit() {
    gpio_set_dir(data_pin, GPIO_OUT);
    gpio_put(data_pin, 0);
    sleep_us(5);
    gpio_set_dir(data_pin, GPIO_IN);
    sleep_us(10);
    bool data = gpio_get(data_pin);
    sleep_us(45);

    return data;
}

bool OneWire::wait_us_for_bit(bool bit, int max_time_us) {
    uint32_t start_time = to_us_since_boot(get_absolute_time());
    while (to_us_since_boot(get_absolute_time()) - start_time < max_time_us) {
        if (gpio_get(data_pin) == bit) {
            return true;
        }

        sleep_us(1);
    }

    return false;
}

bool OneWire::wait_ms_for_bit(bool bit, int max_time_ms) {
    return wait_us_for_bit(bit, max_time_ms * 1000);
}
