#include <stdio.h>
#include "pico/stdlib.h"

#include "rom.hpp"

const int data_pin = 0;

void write_bit(bool value) {
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

bool read_bit() {
    gpio_set_dir(data_pin, GPIO_OUT);
    gpio_put(data_pin, 0);
    sleep_us(5);
    gpio_set_dir(data_pin, GPIO_IN);
    sleep_us(10);
    bool data = gpio_get(data_pin);
    sleep_us(45);

    return data;
}

bool wait_for_bit(bool bit, int max_time_us) {
    uint32_t start_time = to_us_since_boot(get_absolute_time());
    while (to_us_since_boot(get_absolute_time()) - start_time < max_time_us) {
        if (gpio_get(data_pin) == bit) {
            return true;
        }

        sleep_us(1);
    }

    return false;
}

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected()) {
        tight_loop_contents();
    }
    sleep_ms(1000);
    printf("Starting...\n");

    // Initialize data pin
    gpio_init(data_pin);
    gpio_pull_up(data_pin);

    // Write 0 to initialize connection
    gpio_set_dir(data_pin, GPIO_OUT);
    gpio_put(data_pin, 0);
    sleep_us(500);

    // Wait for presence pulse
    gpio_set_dir(data_pin, GPIO_IN);
    sleep_us(5);
    bool detected_presence_pulse = wait_for_bit(0, 240 + 55);
    if (!detected_presence_pulse) {
        printf(":( Did not detect presence pulse!\n");
        return 0;
    }

    // Wait for presence pulse to end
    bool detected_presence_pulse_end = wait_for_bit(1, 240);
    if (!detected_presence_pulse_end) {
        printf(":( Did not detect presence pulse end!\n");
        return 0;
    }

    // Send ROM command
    uint8_t command = 0b00110011;
    for (int i = 0; i < 8; i++) {
        bool bit = (command >> i) & 0x01;
        write_bit(bit);
        sleep_us(5);
    }

    Rom rom;

    // Receive family code
    for (int i = 7; i >= 0; i--) {
        rom.family_code[i] = read_bit() ? '1' : '0';
        sleep_us(5);
    }

    // Receive family code
    for (int i = 47; i >= 0; i--) {
        rom.serial_number[i] = read_bit() ? '1' : '0';
        sleep_us(5);
    }

    // Receive CRC code
    for (int i = 7; i >= 0; i--) {
        rom.crc_code[i] = read_bit() ? '1' : '0';
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
