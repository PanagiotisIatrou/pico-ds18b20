#include <stdio.h>
#include "pico/stdlib.h"

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
    bool detected_presence_pulse = false;
    uint32_t presence_pulse_start_time = to_us_since_boot(get_absolute_time());
    while (to_us_since_boot(get_absolute_time()) - presence_pulse_start_time < 240 + 55) {
        if (gpio_get(data_pin) == 0) {
            detected_presence_pulse = true;
            break;
        }

        sleep_us(1);
    }

    if (!detected_presence_pulse) {
        printf(":( Did not detect presence pulse!\n");
        return 0;
    }

    // Wait for presence pulse to end
    bool detected_presence_pulse_end = false;
    presence_pulse_start_time = to_us_since_boot(get_absolute_time());
    while (to_us_since_boot(get_absolute_time()) - presence_pulse_start_time < 240) {
        if (gpio_get(data_pin) == 1) {
            detected_presence_pulse_end = true;
            break;
        }

        sleep_us(1);
    }

    if (!detected_presence_pulse_end) {
        printf(":( Did not detect presence pulse end!\n");
        return 0;
    }

    // Send ROM command
    char command[8] = "00110011";
    for (int i = 7; i >= 0; i--) {
        bool bit = command[i] == '1';
        write_bit(bit);
        sleep_us(5);
    }

    // Receive family code
    char family_code[8];
    for (int i = 7; i >= 0; i--) {
        family_code[i] = read_bit(i) ? '1' : '0';
        sleep_us(5);
    }

    // Receive family code
    char serial_number[48];
    for (int i = 47; i >= 0; i--) {
        serial_number[i] = read_bit(i) ? '1' : '0';
        sleep_us(5);
    }

    // Receive CRC code
    char crc_code[8];
    for (int i = 7; i >= 0; i--) {
        crc_code[i] = read_bit(i) ? '1' : '0';
        sleep_us(5);
    }

    for (int i = 0; i < 8; i++) {
        printf("%c", family_code[i]);
    }
    printf("\n");
    for (int i = 0; i < 48; i++) {
        printf("%c", serial_number[i]);
    }
    printf("\n");
    for (int i = 0; i < 8; i++) {
        printf("%c", crc_code[i]);
    }
    printf("\n");

    fflush(stdout);
    sleep_ms(1);
    return 0;
}
