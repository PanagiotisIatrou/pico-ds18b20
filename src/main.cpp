#include <stdio.h>
#include "pico/stdlib.h"

#include "rom.hpp"
#include "one_wire.hpp"
#include "ds18b20.hpp"

const int data_pin = 0;

int main()
{
    // Enable stdio
    stdio_init_all();
    while (!stdio_usb_connected()) {
        tight_loop_contents();
    }
    sleep_ms(1000);
    printf("Starting...\n");

    // Initialize a device on the data pin
    OneWire one_wire(data_pin);
    Ds18b20 device = Ds18b20::search_rom(one_wire);
    if (!device.is_valid()) {
        printf("Could not initialize device");
        return 1;
    }

    device.set_resolution(Resolution::VeryHigh, true);
    while (true) {
        float temperature = device.measure_temperature();
        if (temperature == -1000.0) {
            printf("Lost device\n");
            while (!device.ping()) {
                sleep_ms(500);
            }
            printf("Found again\n");
        } else {
            printf("%f\n", temperature);
        }
    }

    fflush(stdout);
    sleep_ms(1000);
    return 0;
}
