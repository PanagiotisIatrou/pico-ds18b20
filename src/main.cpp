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
    etl::vector<Ds18b20, 10> devices = Ds18b20::search_rom(one_wire);
    if (devices.size() == 0) {
        printf("Did not find any devices\n");
        return 0;
    }

    // Set the resolution of the devices
    for (int i = 0; i < devices.size(); i++) {
        bool success = devices[i].set_resolution(Resolution::VeryHigh, true);
        if (!success) {
            printf("Could not set accuracy for a device\n");
            return 1;
        }
    }

    // Continuously calculate and print the temperatures
    while (true) {
        printf("| ");
        for (int i = 0; i < devices.size(); i++) {
            devices[i].ping();
            std::optional<float> result = devices[i].measure_temperature();
            if (result.has_value()) {
                float temperature = result.value();
                printf("%f | ", temperature);
            } else {
                printf("x | ");
            }
        }
        printf("\n");
    }

    fflush(stdout);
    sleep_ms(1000);
    return 0;
}
