#pragma once

#include <stdint.h>

struct Rom {
    uint8_t family_code;
    uint8_t serial_number[6];
    uint8_t crc_code;
};
