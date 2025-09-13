enum class RomCommands {
    ReadRom = 0x33,
    MatchRom = 0x55,
    SkipRom = 0xCC,
    SearchRom = 0xF0
};

enum class FunctionCommands {
    ConvertT = 0x44,
    ReadScratchpad = 0xBE,
    WriteScratchpad = 0x4E,
    CopyScratchpad = 0x48,
    ReadPowerSupply = 0xB4
};
