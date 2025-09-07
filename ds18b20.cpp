#include <stdio.h>
#include "pico/stdlib.h"

#include "rom.hpp"
#include "one_wire.hpp"

const int data_pin = 0;

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected()) {
        tight_loop_contents();
    }
    sleep_ms(1000);
    printf("Starting...\n");

    // Initialize data pin
    OneWire one_wire;
    initialize_one_wire(one_wire, data_pin);

    // Write 0 to initialize connection
    set_state(one_wire, OneWireState::WRITE);
    set_pin_value(one_wire, 0);
    sleep_us(500);

    // Wait for presence pulse
    set_state(one_wire, OneWireState::READ);
    sleep_us(5);
    bool detected_presence_pulse = wait_for_bit(one_wire, 0, 240 + 55);
    if (!detected_presence_pulse) {
        printf(":( Did not detect presence pulse!\n");
        return 0;
    }

    // Wait for presence pulse to end
    bool detected_presence_pulse_end = wait_for_bit(one_wire, 1, 240);
    if (!detected_presence_pulse_end) {
        printf(":( Did not detect presence pulse end!\n");
        return 0;
    }

    // Send ROM command
    uint8_t command = 0b00110011;
    for (int i = 0; i < 8; i++) {
        bool bit = (command >> i) & 0x01;
        write_bit(one_wire, bit);
        sleep_us(5);
    }

    Rom rom;

    // Receive family code
    for (int i = 7; i >= 0; i--) {
        rom.family_code[i] = read_bit(one_wire) ? '1' : '0';
        sleep_us(5);
    }

    // Receive family code
    for (int i = 47; i >= 0; i--) {
        rom.serial_number[i] = read_bit(one_wire) ? '1' : '0';
        sleep_us(5);
    }

    // Receive CRC code
    for (int i = 7; i >= 0; i--) {
        rom.crc_code[i] = read_bit(one_wire) ? '1' : '0';
        sleep_us(5);
    }

    for (int i = 0; i < 8; i++) {
        printf("%c", rom.family_code[i]);
    }
    printf("\n");
    for (int i = 0; i < 48; i++) {
        printf("%c", rom.serial_number[i]);
    }
    printf("\n");
    for (int i = 0; i < 8; i++) {
        printf("%c", rom.crc_code[i]);
    }
    printf("\n");

    fflush(stdout);
    sleep_ms(1000);
    return 0;
}
