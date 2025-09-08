#include "device.hpp"

#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"

Device::Device(OneWire& one_wire) : one_wire(one_wire) { }

bool Device::presence_pulse() {
    // Write 0 to initialize connection
    one_wire.set_state(OneWireState::WRITE);
    one_wire.set_pin_value(0);
    sleep_us(500);

    // Wait for presence pulse
    one_wire.set_state(OneWireState::READ);
    sleep_us(5);
    bool detected_presence_pulse = one_wire.wait_for_bit(0, 240 + 55);
    if (!detected_presence_pulse) {
        return false;
    }

    // Wait for presence pulse to end
    bool detected_presence_pulse_end = one_wire.wait_for_bit(1, 240);
    if (!detected_presence_pulse_end) {
        return false;
    }

    return true;
}

void Device::send_command(const char command[8]) {
    for (int i = 7; i >= 0; i--) {
        bool bit = command[i] == '1';
        one_wire.write_bit(bit);
        sleep_us(5);
    }
}

void Device::skip_rom() {
    send_command("11001100");
}

void Device::read_rom() {
    send_command("00110011");

    // Receive family code
    for (int i = 7; i >= 0; i--) {
        rom.family_code[i] = one_wire.read_bit() ? '1' : '0';
        sleep_us(5);
    }

    // Receive family code
    for (int i = 47; i >= 0; i--) {
        rom.serial_number[i] = one_wire.read_bit() ? '1' : '0';
        sleep_us(5);
    }

    // Receive CRC code
    for (int i = 7; i >= 0; i--) {
        rom.crc_code[i] = one_wire.read_bit() ? '1' : '0';
        sleep_us(5);
    }
}

void Device::match_rom() {
    send_command("01010101");

    // Send family code
    for (int i = 7; i >= 0; i--) {
        one_wire.write_bit(rom.family_code[i]);
        sleep_us(5);
    }

    // Send family code
    for (int i = 47; i >= 0; i--) {
        one_wire.write_bit(rom.serial_number[i]);
        sleep_us(5);
    }

    // Send CRC code
    for (int i = 7; i >= 0; i--) {
        one_wire.write_bit(rom.crc_code[i]);
        sleep_us(5);
    }
}
