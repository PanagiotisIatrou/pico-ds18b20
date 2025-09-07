#include <stdio.h>
#include "pico/stdlib.h"

const int data_pin = 0;

void write_value(bool value) {
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
    sleep_us(65); // 10uS for rising edge + 15-60uS for slave waiting
    int max_presence_pulse_wait_time = 240;
    bool detected_presence_pulse = false;
    uint32_t presence_pulse_start_time = to_us_since_boot(get_absolute_time());
    while (!detected_presence_pulse) {
        if (to_us_since_boot(get_absolute_time()) - presence_pulse_start_time > max_presence_pulse_wait_time) {
            break;
        }

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

    printf("Detected presence pulse!\n");

    // Wait for presence pulse to end
    bool detected_presence_pulse_end = false;
    presence_pulse_start_time = to_us_since_boot(get_absolute_time());
    while (gpio_get(data_pin) == 0) {
        if (to_us_since_boot(get_absolute_time()) - presence_pulse_start_time > max_presence_pulse_wait_time) {
            break;
        }

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

    printf("Detected presence pulse end!\n");

    // Send ROM command
    char command[8] = "00110011";
    for (int i = 7; i >= 0; i--) {
        bool bit = command[i] == '1';
        write_value(bit);
        sleep_us(5);
    }

    printf("Wrote ROM command!\n");
}
