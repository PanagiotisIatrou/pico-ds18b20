#pragma once

#include <stdint.h>

class Rom {
private:
    uint8_t m_family_code;
    uint8_t m_serial_number[6];
    uint8_t m_crc_code;

public:
    Rom();

    Rom(uint8_t family_code, uint8_t serial_number[6], uint8_t crc_code);

    uint8_t get_family_code();

    uint8_t get_serial_number(int index);

    uint8_t get_crc_code();

    bool is_empty();

    bool has_valid_crc();

    static uint64_t encode_rom(Rom encoded_rom);

    static Rom decode_rom(uint64_t encoded_rom);

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

    bool operator!=(const Rom& other) {
        return !(*this == other);
    }
};
