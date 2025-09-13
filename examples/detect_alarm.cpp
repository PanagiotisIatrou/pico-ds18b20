#include <stdio.h>
#include "pico/stdlib.h"

#include "one_wire.hpp"
#include "ds18b20.hpp"

int main()
{
    // Enable stdio and wait for serial monitor to connect
    stdio_init_all();
    while (!stdio_usb_connected()) {
        tight_loop_contents();
    }
    sleep_ms(1000);
    printf("Starting...\n");

    // Find all the devices on the data pin (0)
    OneWire one_wire(0);
    etl::vector<Ds18b20, 10> devices = Ds18b20::find_devices(one_wire);
    if (devices.size() == 0) {
        printf("Did not find any devices\n");
        return 0;
    }

    // Set the temperature alarm limits of the devices
    for (int i = 0; i < devices.size(); i++) {
        bool success0 = devices[i].set_temperature_low_limit(-20, true);
        bool success1 = devices[i].set_temperature_high_limit(80, true);
        if (!success0 || !success1) {
            printf("Could not set temperature limits for a device\n");
            return 1;
        }
    }

    for (int i = 0; i < devices.size(); i++) {
        std::optional<float> result = devices[i].measure_temperature();
        if (result.has_value()) {
            float temperature = result.value();
            printf("%f | ", temperature);
        } else {
            printf("x | ");
        }
        if (devices[i].is_alarm_active()) {
            printf("ALARM!!!\n");
        } else {
            printf("All safe!\n");
        }
    }

    fflush(stdout);
    sleep_ms(1000);
    return 0;
}
