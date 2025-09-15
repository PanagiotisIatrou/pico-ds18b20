#pragma once

#include "device_commands.hpp"

#include "etl/vector.h"

/**
 * Contains the functionality of a ds18b20 device.
 */
class Ds18b20 {
private:
    OneWire& m_one_wire; ///< OneWire Object responsible for all OneWire communication.

    Rom m_rom; ///< The Rom of the device

    Scratchpad m_scratchpad; ///< The scratchpad of the device

    bool is_initialized = false; ///< The state of the device after initialization (constructor called).
    
    static const int m_max_tries = 10; ///< The maximum amount of tries before a command fails (indicates device failure).

    /**
     * Overwrites the scratchpad with the parameter values. Also saves it to the EEPROM if specified.
     * @param temperature_high_limit The upper temperature limit for triggering the alarm.
     * @param temperature_low_limit The lower temperature limit for triggering the alarm.
     * @param configuration Byte containing 2-bits indicating the resolution of the measurements.
     * @param save Whether to save the scratchpad to the EEPROM after writing to it.
     * @return True if the writing was successful, false if not.
     */
    bool set_scratchpad(int8_t temperature_high_limit, int8_t temperature_low_limit, uint8_t configuration, bool save);

public:
    /**
     * Creates a Ds18b20 object configured to the specified OneWire and Rom., and reads its scratchpad. If the read
     * is successful is_initialized is set to true, if not, it is set to false.
     */
    Ds18b20(OneWire& one_wire, Rom rom);

    /**
     * @return True if the device initialized correctly, false if not.
     */
    bool is_successfully_initialized();

    /**
     * Pings the device to check if it is operational. This is done by reading its Rom and seeing if it matches
     * with the one it was initialized with.
     * @return True if the device is operational, false if not.
     */
    bool ping();

    /**
     * Conducts a temperature measurement on the device.
     * @return If the measurement was successful, the temperature of the measurement is returned. If it
     * failed, std::nullopt is returned.
     */
    std::optional<float> measure_temperature();

    /**
     * @return The resolution of the temperature measurements.
     */
    Resolution get_resolution();

    /**
     * Sets the resolution of the temperature measurements. Also, writes the value to the EEPROM if specified.
     * @param save Writes the scratchpad to the EEPROM if true
     * @return True if the writing was successful, false if not.
     */
    bool set_resolution(Resolution resolution, bool save);

    /**
     * @return The lower temperature limit for triggering the alarm.
     */
    int8_t get_temperature_low_limit();

    /**
     * @return The upper temperature limit for triggering the alarm.
     */
    int8_t get_temperature_high_limit();

    /**
     * Sets the lower temperature limit for triggering the alarm. Also, writes the value to the EEPROM if specified.
     * @return True if the writing was successful, false if not.
     */
    bool set_temperature_low_limit(int8_t temperature, bool save);

    /**
     * Sets the higher temperature limit for triggering the alarm. Also, writes the value to the EEPROM if specified.
     * @return True if the writing was successful, false if not.
     */
    bool set_temperature_high_limit(int8_t temperature, bool save);

    /**
     * Checks if the temperature alarm flag is raised. if it is raised, it means that the last measured temperature
     * was out of bounds (out of the specified limits).
     * @return True if the last temperature was out of the specified limits, false if not.
     */
    bool is_alarm_active();

    /**
     * Reads the power supply mode of the device.
     * @return If the read is successful, the power supply mode (External or Parasite) is returned. If it
     * failed, std::nullopt is returned.
     */
    std::optional<PowerSupplyMode> get_power_supply_mode();

    /**
     * Scans the GPIO pin specified in the OneWire object for connected devices. There can be more than one in a specific GPIO,
     * so a vector of Ds18b20 is returned instead.
     * @param one_wire The OneWire object to act upon.
     * @return A vector with all Ds18b20 devices connected on the GPIO pin specified in OneWire object.
     */
    static etl::vector<Ds18b20, 10> find_devices(OneWire& one_wire);
};
