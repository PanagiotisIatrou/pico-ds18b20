#pragma once

#include <stdint.h>

/**
 * Contains functionality for the 1-Wire protocol used for ds18b20 communication.
 */
class OneWire {
private:
    int m_data_pin; ///< The GPIO used for data communication

    /**
     * Reads whether the data pin is set to low or high.
     * @return The value of the data pin.
     */
    bool get_pin_value();

    /**
     * Sets the value of the data pin to the value parameter
     * @param value
     */
    void set_pin_value(bool value);

    /**
     * Waits until the value of the data pin is equal to the bit parameter or until max_time_us microseconds
     * have passed.
     * @param bit The value we wait for the data pin to reach.
     * @param max_time_us The maximum amount of microseconds to wait.
     * @return True if the data pin turned equal to the bit parameter within max_time_us microseconds, false if not.
     */
    bool wait_us_for_bit(bool bit, int max_time_us);

    /**
     * Waits until the value of the data pin is equal to the bit parameter or until max_time_us milliseconds
     * have passed.
     * @param bit The value we wait for the data pin to reach.
     * @param max_time_us The maximum amount of milliseconds to wait.
     * @return True if the data pin turned equal to the bit parameter within max_time_us milliseconds, false if not.
     */
    bool wait_ms_for_bit(bool bit, int max_time_ms);

public:
    /**
     * Creates a OneWire object operating on data_pin. Also, initializes this GPIO
     * and activates its pull-up resistor.
     * @param data_pin The GPIO used for data communication.
     */
    OneWire(int data_pin);

    /**
     * Issues a write slot and writes the given bit to the bus.
     * @param value The value to write to the bus.
     */
    void write_bit(bool value);

    /**
     * Issues a read slot and reads the value of the bus.
     * @return The value of the bus in this read slot.
     */
    bool read_bit();

    /**
     * Issues 8 write slots and writes the given byte to the bus.
     * @param value 8-bits of data to be sent. LSB first (LSB is the rightmost bit).
     */
    void write_byte(uint8_t value);

    /**
     * Issues 8 read slots and reads the value of the bus.
     * @return The 8-bits of data that were read. LSB first (LSB is the rightmost bit).
     */
    uint8_t read_byte();

    /**
     * Calcualtes the new CRC value, taking the byte parameter into the CRC calculation.
     * @param crc The current crc value: 0 if this is the first calculation, the previous crc value if not.
     * @param byte The byte to include into the crc calculation.
     * @return The updated crc value taking the byte parameter into account.
     */
    static uint8_t calculate_crc_byte(uint8_t crc, uint8_t byte);

    /**
     * Writes 0 to the bus and waits for any device to send the presence pulse. After, it waits for the response to stop.
     * @return True if any ds18b20 using the specified data pin responded, false if not.
     */
    bool reset();
};
