#include "scratchpad.hpp"

#include "one_wire.hpp"

Scratchpad::Scratchpad() {
    for (int i = 0; i < 2; i++) {
        m_temperature[i] = 0;
    }
    m_temperature_high_limit = 0;
    m_temperature_low_limit = 0;
    m_configuration = 0;
    for (int i = 0; i < 3; i++) {
        m_reserved[i] = 0;
    }
    m_crc_code = 0;
}

Scratchpad::Scratchpad(uint8_t temperature[2], int8_t temperature_high_limit, int8_t temperature_low_limit, uint8_t configuration, uint8_t reserved[3], uint8_t crc_code) {
    for (int i = 0; i < 2; i++) {
        m_temperature[i] = temperature[i];
    }
    m_temperature_high_limit = temperature_high_limit;
    m_temperature_low_limit = temperature_low_limit;
    m_configuration = configuration;
    for (int i = 0; i < 3; i++) {
        m_reserved[i] = reserved[i];
    }
    m_crc_code = crc_code;
}

uint8_t Scratchpad::get_temperature_byte(int index) {
    return m_temperature[index];
}

int8_t Scratchpad::get_temperature_high_limit() {
    return m_temperature_high_limit;
}

int8_t Scratchpad::get_temperature_low_limit() {
    return m_temperature_low_limit;
}

uint8_t Scratchpad::get_configuration() {
    return m_configuration;
}

uint8_t Scratchpad::get_crc_code() {
    return m_crc_code;
}

float Scratchpad::calculate_temperature() {
    uint8_t config_setting = get_config_setting();
    int16_t temperature_data = m_temperature[0] + (m_temperature[1] << 8);
    temperature_data = temperature_data >> (3 - config_setting);
    float temperature = temperature_data / (float)(1 << (config_setting + 1));
    return temperature;
}

uint8_t Scratchpad::get_config_setting() {
    return (m_configuration & 0b01100000) >> 5;
}

uint8_t Scratchpad::resolution_to_configuration(Resolution resolution) {
    return 0b00011111 | ((uint8_t)resolution << 5);
}

int Scratchpad::get_resolution() {
    return 9 + get_config_setting();
}

bool Scratchpad::has_valid_crc() {
    uint8_t crc = 0;
    for (int i = 0; i < 2; i++) {
        crc = OneWire::calculate_crc_byte(crc, m_temperature[i]);
    }
    crc = OneWire::calculate_crc_byte(crc, m_temperature_high_limit);
    crc = OneWire::calculate_crc_byte(crc, m_temperature_low_limit);
    crc = OneWire::calculate_crc_byte(crc, m_configuration);
    for (int i = 0; i < 3; i++) {
        crc = OneWire::calculate_crc_byte(crc, m_reserved[i]);
    }
    return crc == m_crc_code;
}
