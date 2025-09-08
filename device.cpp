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
    bool detected_presence_pulse = one_wire.wait_us_for_bit(0, 240 + 55);
    if (!detected_presence_pulse) {
        return false;
    }

    // Wait for presence pulse to end
    bool detected_presence_pulse_end = one_wire.wait_us_for_bit(1, 240);
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

    rom.family_code = 0;
    for (int i = 0; i < 6; i++) {
        rom.serial_number[i] = 0;
    }
    rom.crc_code = 0;

    // Receive family code
    for (int i = 0; i < 8; i++) {
        rom.family_code |= (one_wire.read_bit() << i);
        sleep_us(5);
    }

    // Receive family code
    for (int k = 5; k >= 0; k--) {
        for (int i = 0; i < 8; i++) {
            rom.serial_number[k] |= (one_wire.read_bit() << i);
            sleep_us(5);
        }
    }

    // Receive CRC code
    for (int i = 0; i < 8; i++) {
        rom.crc_code |= (one_wire.read_bit() << i);
        sleep_us(5);
    }
}

void Device::match_rom() {
    send_command("01010101");

    // Send family code
    for (int i = 0; i < 8; i++) {
        bool bit = (rom.family_code >> i) & 0x01;
        one_wire.write_bit(bit);
        sleep_us(5);
    }

    // Send serial number
    for (int k = 5; k >= 0; k--) {
        for (int i = 0; i < 8; i++) {
            bool bit = (rom.serial_number[k] >> i) & 0x01;
            one_wire.write_bit(bit);
            sleep_us(5);
        }
    }

    // Send CRC code
    for (int i = 0; i < 8; i++) {
        bool bit = (rom.crc_code >> i) & 0x01;
        one_wire.write_bit(bit);
        sleep_us(5);
    }
}

void Device::convert_t() {
    send_command("01000100");

    bool detected = false;
    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    while (to_ms_since_boot(get_absolute_time()) - start_time < 1000) {
        bool value = one_wire.read_bit();
        if (value) {
            detected = true;
            break;
        }
        sleep_ms(5);
    }
    if (detected) {
        printf("Finished temp %d\n", to_ms_since_boot(get_absolute_time()) - start_time);
    } else {
        printf(":( No temp\n");
    }
}
