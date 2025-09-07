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

    // Initialize data pin
    OneWire one_wire(data_pin);

    Device device(one_wire);

    if (!device.presence_pulse()) {
        printf("Did not detect presence pulse\n");
        return 0;
    }

    // Send ROM command
    device.send_rom_command("00110011");

    for (int i = 0; i < 8; i++) {
        printf("%c", device.rom.family_code[i]);
    }
    printf("\n");
    for (int i = 0; i < 48; i++) {
        printf("%c", device.rom.serial_number[i]);
    }
    printf("\n");
    for (int i = 0; i < 8; i++) {
        printf("%c", device.rom.crc_code[i]);
    }
    printf("\n");

    fflush(stdout);
    sleep_ms(1000);
    return 0;
}
