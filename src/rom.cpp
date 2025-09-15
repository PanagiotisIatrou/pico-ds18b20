#include "rom.hpp"

#include "one_wire.hpp"

Rom::Rom() {
    m_family_code = 0;
    for (int i = 0; i < 6; i++) {
        m_serial_number[i] = 0;
    }
    m_crc_code = 0;
}

Rom::Rom(uint8_t family_code, uint8_t serial_number[6], uint8_t crc_code) {
    m_family_code = family_code;
    for (int i = 0; i < 6; i++) {
        m_serial_number[i] = serial_number[i];
    }
    m_crc_code = crc_code;
}

uint8_t Rom::get_family_code() const {
    return m_family_code;
}

uint8_t Rom::get_serial_number(int index) const {
    return m_serial_number[index];
}

uint8_t Rom::get_crc_code() const {
    return m_crc_code;
}

bool Rom::is_empty() const {
    if (m_family_code != 0) {
        return false;
    }
    for (int i = 0; i < 6; i++) {
        if (m_serial_number[i] != 0) {
            return false;
        }
    }
    if (m_crc_code != 0) {
        return false;
    }

    return true;
}

bool Rom::has_valid_crc() const {
    uint8_t crc = 0;
    crc = OneWire::calculate_crc_byte(crc, m_family_code);
    for (int i = 0; i < 6; i++) {
        crc = OneWire::calculate_crc_byte(crc, m_serial_number[i]);
    }
    return crc == m_crc_code;
}

uint64_t Rom::encode_rom(Rom decoded_rom) {
    uint64_t rom = 0;
    rom |= ((uint64_t)decoded_rom.get_family_code());
    for (int i = 0; i < 6; i++) {
        rom |= (((uint64_t)decoded_rom.get_serial_number(i)) << ((i + 1) * 8));
    }
    rom |= (((uint64_t)decoded_rom.get_crc_code()) << 56);
    return rom;
}

Rom Rom::decode_rom(uint64_t encoded_rom) {
    uint8_t family_code = encoded_rom & 0xFF;
    uint8_t serial_number[6];
    for (int i = 0; i < 6; i++) {
        serial_number[i] = (encoded_rom >> (8 * (i + 1))) & 0xFF;
    }
    uint8_t crc_code = (encoded_rom >> 56) & 0xFF;
    return Rom(family_code, serial_number, crc_code);
}
