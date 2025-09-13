#pragma once

#include <stdint.h>

struct Rom {
    uint8_t family_code;
    uint8_t serial_number[6];
    uint8_t crc_code;
};

inline bool operator==(const Rom& lhs, const Rom& rhs) {
    if (lhs.family_code != rhs.family_code) {
        return false;
    }
    for (int i = 0; i < 6; i++) {
        if (lhs.serial_number[i] != rhs.serial_number[i]) {
            return false;
        }
    }
    if (lhs.crc_code != rhs.crc_code) {
        return false;
    }

    return true;
}

inline bool operator!=(const Rom& lhs, const Rom& rhs) {
    return !(lhs == rhs);
}
