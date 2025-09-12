#pragma once

#include <stdint.h>

struct Scratchpad {
    uint8_t temperature[2];
    int8_t temperature_high_limit;
    int8_t temperature_low_limit;
    uint8_t configuration;
    uint8_t reserved[3];
    uint8_t crc_code;
};
