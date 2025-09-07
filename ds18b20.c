#include <stdio.h>
#include "pico/stdlib.h"

const int data_pin = 0;

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected()) {
        tight_loop_contents();
    }
    sleep_ms(1000);
    printf("Starting...\n");

    // int count = 0;
    // while (true) {
    //     count++;
    //     printf("Hello, world! %d\n", count);
    //     sleep_ms(1000);
    // }

    // Initialize data pin
    gpio_init(data_pin);
    gpio_pull_up(data_pin);

    // Write 0 to initialize connection
    gpio_set_dir(data_pin, GPIO_OUT);
    gpio_put(data_pin, 0);
    sleep_us(500);

    // Wait for presense pulse
    gpio_set_dir(data_pin, GPIO_IN);
    sleep_us(70); // 10uS for rising edge + 15-60uS for slave waiting
    bool presense_pulse = gpio_get(data_pin);
    if (!presense_pulse) {
        printf("Detected presense pulse!!\n");
    } else {
        printf("Nooooo! :(\n");
    }
}
