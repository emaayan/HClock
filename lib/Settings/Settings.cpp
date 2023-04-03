#include <Settings.h>
#include <EEPROM.h>

uint8_t readSetting(int addr)
{
    return EEPROM.read(addr);
};

void writeSetting(int addr, uint8_t value)
{
    if (value < UNREADVALUE)
    {
        EEPROM.write(addr, value);
    }
};

uint8_t initSetting(int addr, int def)
{
    uint8_t setting = readSetting(addr);
    if (setting == UNREADVALUE)
    {
        setting = def;
        writeSetting(addr, setting);
    }
    return setting;
};