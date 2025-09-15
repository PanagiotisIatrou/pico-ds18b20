#pragma once

#include <stdint.h>

enum class Resolution { Low = 0b00, Medium = 0b01, High = 0b10, VeryHigh = 0b11 };

/**
 * Contains functionality for accessing the 72-bit Scratchpad contained in each ds18b20
 */
class Scratchpad {
private:
    uint8_t m_temperature[2]; ///< 2 bytes of the temperature measurement (index 0 is LSB)
    int8_t m_temperature_high_limit; ///< The upper temperature limit for triggering the alarm
    int8_t m_temperature_low_limit; ///< The lower temperature limit for triggering the alarm
    uint8_t m_configuration; ///< Byte containing 2-bits indicating the resolution of the measurements
    uint8_t m_reserved[3]; ///< 3 bytes reserved by the ds18b20
    uint8_t m_crc_code; ///< The 8-bit CRC code of the scratchpad

    /**
     * Extracts the 2 important bits from the configuration indicating the resolution.
     * @return 0, 1, 2 or 3, indicating 9, 10, 11 or 12 bit resolution.
     */
    uint8_t get_config_setting();

public:
    /**
     * Creates an empty scratchpad (all bits set to 0).
     */
    Scratchpad();

    /**
     * Creates a Scratchpad and fills it with the parameters.
     * @param temperature The temperature measurement.
     * @param temperature_high_limit The upper temperature limit for triggering the alarm.
     * @param temperature_low_limit The lower temperature limit for triggering the alarm.
     * @param configuration Byte indicating the resolution of the measurements.
     * @param reserved Reserved by the ds18b20.
     * @param crc_code The CRC code of the scratchpad.
     */
    Scratchpad(uint8_t temperature[2], int8_t temperature_high_limit, int8_t temperature_low_limit, uint8_t configuration, uint8_t reserved[3], uint8_t crc_code);

    /**
     * @param index The index of the byte to return.
     * @return the index-th byte of the temperature measurement.
     */
    uint8_t get_temperature_byte(int index);

    /**
     * @return The upper temperature limit for triggering the alarm.
     */
    int8_t get_temperature_high_limit();

    /**
     * @return The lower temperature limit for triggering the alarm.
     */
    int8_t get_temperature_low_limit();

    /**
     * @return The configuration byte containing the resolution.
     */
    uint8_t get_configuration();

    /**
     * @return The CRC code of the Scratchpad.
     */
    uint8_t get_crc_code();

    /**
     * Converts the 2 bytes of the temperature measurement to a float number.
     * @return The temperature measurement in a readable format.
     */
    float calculate_temperature();

    /**
     * Converts the given resolution to a configuration byte.
     * For example, Low -> 00011111, Medium -> 00111111, High -> 01011111, VeryHigh -> 01111111.
     * @param resolution A setting for 9, 10, 11 or 12 bit resolution.
     * @return A byte containing 2 bytes indicating the specified resolution.
     */
    uint8_t resolution_to_configuration(Resolution resolution);

    /**
     * Extracts the resolution from the configuration byte
     * @return The resolution (9, 10, 11 or 12).
     */
    int get_resolution();

    /**
     * Calculates the CRC code from the temperature, temperature_high_limit, temperature_low_limit,
     * configuration and reserved and checks if it matches the Rom CRC code.
     * @return true if the match, false if not.
     */
    bool has_valid_crc();
};
