#pragma once

#include <optional>

#include "rom.hpp"
#include "scratchpad.hpp"
#include "one_wire.hpp"

enum class PowerSupplyMode { Parasite, External };

/**
 * Contains all ds18b20 rom/function commands.
 */
class DeviceCommands {
public:
    /// The return type for search commands (search_rom, search_alarm)
    struct SearchInfo {
        Rom rom;
        uint64_t last_choice_path = 0;
        int last_choice_path_size = -1;
    };
    
private:
    // OneWire& m_one_wire; ///< OneWire Object responsible for all OneWire communication

    /**
     * Conducts a search for a matching Rom according to the search function command (search_rom, search_alarm).
     * @param one_wire A reference to a OneWire object to act upon.
     * @param previous_sequence The path of the previous search that led to a choice. 0 if no other searches have been conducted.
     * @param previous_sequence_length The length of the path of the previous search that led to a choice. 0 if no other searches have been conducted.
     * @return If the search was successful, a Rom, the path just before the last choice and that path's length are returned.
     * If the search failed, std::nullopt is returned.
     */
    static std::optional<SearchInfo> search(OneWire& one_wire, uint64_t previous_sequence, int previous_sequence_length);

public:
    // ROM commands

    /**
     * Selects the only ds18b20 device connected to act upon on the next function command.
     * Only works when only 1 ds18b20 is connected on the same data pin (when it is the only device using
     * this specific OneWire object).
     */
    static void skip_rom(OneWire& one_wire);

    /**
     * Reads the Rom of the only ds18b20 device connected.
     * Only works when only 1 ds18b20 is connected on the same data pin (when it is the only device using
     * this specific OneWire object).
     * @param one_wire A reference to a OneWire object to act upon.
     * @return The Rom object that was just read.
     */
    static std::optional<Rom> read_rom(OneWire& one_wire);

    /**
     * Selects the Rom of this device as the Rom to act upon on the next function command.
     */
    static void match_rom(OneWire& one_wire, Rom& rom);

    /**
     * Conducts a search for a matching Rom.
     * @param one_wire A reference to a OneWire object to act upon.
     * @param previous_sequence The path of the previous search that led to a choice. 0 if no other searches have been conducted.
     * @param previous_sequence_length The length of the path of the previous search that led to a choice. 0 if no other searches have been conducted.
     * @return If the search was successful, a Rom, the path just before the last choice and that path's length are returned.
     * If the search failed, std::nullopt is returned.
     */
    static std::optional<SearchInfo> search_rom(OneWire& one_wire, uint64_t previous_sequence, int previous_sequence_length);

    /**
     * Conducts a search for a matching Rom which has its alarm flag raised.
     * @param one_wire A reference to a OneWire object to act upon.
     * @param previous_sequence The path of the previous search that led to a choice. 0 if no other searches have been conducted.
     * @param previous_sequence_length The length of the path of the previous search that led to a choice. 0 if no other searches have been conducted.
     * @return If the search was successful, a Rom, the path just before the last choice and that path's length are returned.
     * If the search failed, std::nullopt is returned.
     */
    static std::optional<SearchInfo> search_alarm(OneWire& one_wire, uint64_t previous_sequence, int previous_sequence_length);

    // Function commands

    /**
     * Conducts a temperature measurement on the selected device.
     * @return If the measurement is successful, the time it took in milliseconds is returned.
     * If the measurement failed, std::nullopt is returned.
     */
    static std::optional<uint32_t> convert_t(OneWire& one_wire);

    /**
     * Reads the scratchpad of the selected device and stores it into the Rom object of this device.
     * @return True if the read was successful, false if not.
     */
    static std::optional<Scratchpad> read_scratchpad(OneWire& one_wire);

    /**
     * Overwrites the scratchpad with the parameter values.
     * @param temperature_high The upper temperature limit for triggering the alarm.
     * @param temperature_low The lower temperature limit for triggering the alarm.
     * @param configuration Byte containing 2-bits indicating the resolution of the measurements.
     */
    static void write_scratchpad(OneWire& one_wire, int8_t temperature_high, int8_t temperature_low, uint8_t configuration);

    /**
     * Writes the scratchpad to the EEPROM.
     * @return If the writing is successful, the time it took in milliseconds is returned.
     * If the writing failed, std::nullopt is returned.
     */
    static std::optional<uint32_t> copy_scratchpad(OneWire& one_wire);

    /**
     * Fetches the power supply mode of the selected device.
     * @return The power supply mode (External or Parasite)
     */
    static PowerSupplyMode read_power_supply_mode(OneWire& one_wire);
};
