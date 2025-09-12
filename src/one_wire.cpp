#include "one_wire.hpp"

#include "pico/stdlib.h"

OneWire::OneWire(int data_pin) : m_data_pin(data_pin) {
    gpio_init(data_pin);
    gpio_pull_up(data_pin);
}

void OneWire::set_state(OneWireState state) {
    state = state;
    bool gpio_state = state == OneWireState::READ ? GPIO_IN : GPIO_OUT;
    gpio_set_dir(m_data_pin, gpio_state);
}

OneWireState OneWire::get_state() {
    return m_state;
}

bool OneWire::get_pin_value() {
    return gpio_get(m_data_pin);
}

void OneWire::set_pin_value(bool value) {
    gpio_put(m_data_pin, value);
}

void OneWire::write_bit(bool value) {
    gpio_set_dir(m_data_pin, GPIO_OUT);
    gpio_put(m_data_pin, 0);
    if (value) {
        sleep_us(8);
        gpio_set_dir(m_data_pin, GPIO_IN);
        sleep_us(52);
    } else {
        sleep_us(60);
        gpio_set_dir(m_data_pin, GPIO_IN);
    }
    sleep_us(5);
}

bool OneWire::read_bit() {
    gpio_set_dir(m_data_pin, GPIO_OUT);
    gpio_put(m_data_pin, 0);
    sleep_us(5);
    gpio_set_dir(m_data_pin, GPIO_IN);
    sleep_us(10);
    bool data = gpio_get(m_data_pin);
    sleep_us(50);

    return data;
}

void OneWire::write_byte(uint8_t value) {
    for (int i = 0; i < 8; i++) {
        bool bit = (value >> i) & 0x01;
        write_bit(bit);
    }
}

uint8_t OneWire::read_byte() {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        byte |= (read_bit() << i);
    }

    return byte;
}

bool OneWire::wait_us_for_bit(bool bit, int max_time_us) {
    uint32_t start_time = to_us_since_boot(get_absolute_time());
    while (to_us_since_boot(get_absolute_time()) - start_time < max_time_us) {
        if (gpio_get(m_data_pin) == bit) {
            return true;
        }

        sleep_us(1);
    }

    return false;
}

bool OneWire::wait_ms_for_bit(bool bit, int max_time_ms) {
    return wait_us_for_bit(bit, max_time_ms * 1000);
}

uint8_t OneWire::calculate_crc_byte(uint8_t crc, uint8_t byte) {
    crc ^= byte;
    for (int i = 0; i < 8; i++) {
        if (crc & 0x01) {
            crc = (crc >> 1) ^ 0x8C;
        } else {
            crc >>= 1;
        }
    }

    return crc;
}

bool OneWire::reset() {
    // Write 0 to initialize connection
    set_state(OneWireState::WRITE);
    set_pin_value(0);
    sleep_us(500);

    // Wait for presence pulse
    set_state(OneWireState::READ);
    sleep_us(5);
    bool detected_presence_pulse = wait_us_for_bit(0, 240 + 55);
    if (!detected_presence_pulse) {
        return false;
    }

    // Wait for presence pulse to end
    bool detected_presence_pulse_end = wait_us_for_bit(1, 240);
    if (!detected_presence_pulse_end) {
        return false;
    }

    return true;
}
