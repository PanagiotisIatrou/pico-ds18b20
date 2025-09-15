#pragma once

#include <stdint.h>

/**
 * Contains functionality for accessing the 64-bit Rom contained in each ds18b20
 */
class Rom {
private:
    uint8_t m_family_code; ///< The 8-bit family code of the device.
    uint8_t m_serial_number[6]; ///< The 48-bit serial number of the device.
    uint8_t m_crc_code; ///< The 8-bit CRC code of the Rom.

public:
    /**
     * Creates an empty Rom (64 bits set to 0).
     */
    Rom();

    /**
     * Creates a Rom and fills it with the parameters.
     * @param family_code The family code of the device.
     * @param serial_number The serial number of the device.
     * @param crc_code The CRC code of the Rom.
     */
    Rom(uint8_t family_code, uint8_t serial_number[6], uint8_t crc_code);

    /**
     * @return The family code of the device.
     */
    uint8_t get_family_code();

    /**
     * @param index The index of the byte of the serial number (index 0 is the LSB)
     * @return The index-th byte of the serial number of the device.
     */
    uint8_t get_serial_number(int index);

    /**
     * @return The CRC code of the Rom.
     */
    uint8_t get_crc_code();

    /**
     * Checks if the Rom contains all zeros.
     * @return true if all zeros, false if not.
     */
    bool is_empty();

    /**
     * Calculates the CRC code from the family code and the serial number and checks if
     * it matches the Rom CRC code.
     * @return true if the match, false if not.
     */
    bool has_valid_crc();

    /**
     * Turns a Rom object into uint64_t
     * @param decoded_rom A Rom object
     * @return The 64-bit representation of decoded_rom
     */
    static uint64_t encode_rom(Rom decoded_rom);

    /**
     * Turns uint64_t into a Rom object
     * @param encoded_rom A 64-bit representation of a Rom
     * @return The Rom object representation of encoded_rom
     */
    static Rom decode_rom(uint64_t encoded_rom);

    /**
     * Checks if 2 Roms are equal
     */
    bool operator==(const Rom& other) const {
        if (m_family_code != other.m_family_code) {
            return false;
        }
        for (int i = 0; i < 6; i++) {
            if (m_serial_number[i] != other.m_serial_number[i]) {
                return false;
            }
        }
        if (m_crc_code != other.m_crc_code) {
            return false;
        }

        return true;
    }

    /**
     * Checks if 2 Roms are not equal
     */
    bool operator!=(const Rom& other) {
        return !(*this == other);
    }
};
