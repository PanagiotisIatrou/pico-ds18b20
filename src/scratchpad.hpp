#pragma once

#include <stdint.h>

enum class Resolution { Low = 0b00, Medium = 0b01, High = 0b10, VeryHigh = 0b11 };

class Scratchpad {
private:
    uint8_t m_temperature[2];
    int8_t m_temperature_high_limit;
    int8_t m_temperature_low_limit;
    uint8_t m_configuration;
    uint8_t m_reserved[3];
    uint8_t m_crc_code;

    uint8_t get_config_setting();

public:
    Scratchpad();

    Scratchpad(uint8_t temperature[2], int8_t temperature_high_limit, int8_t temperature_low_limit, uint8_t configuration, uint8_t reserved[3], uint8_t crc_code);

    uint8_t get_temperature_byte(int index);

    int8_t get_temperature_high_limit();

    int8_t get_temperature_low_limit();

    uint8_t get_configuration();

    uint8_t get_crc_code();

    float calculate_temperature();

    uint8_t resolution_to_configuration(Resolution resolution);

    int get_resolution();

    bool has_valid_crc();
};
