#include <stdio.h>
#include "pico/stdlib.h"

#include "rom.hpp"
#include "one_wire.hpp"
#include "device.hpp"

const int data_pin = 0;

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected()) {
        tight_loop_contents();
    }
    sleep_ms(1000);
    printf("Starting...\n");

    // Initialize a device on the data pin
    OneWire one_wire(data_pin);
    Device device(one_wire);

    device.set_resolution(Resolution::VeryHigh, true);
    while (true) {
        float temperature = device.measure_temperature();
        printf("%f\n", temperature);
    }

    fflush(stdout);
    sleep_ms(1000);
    return 0;
}
