#include <Settings.h>
#include <EEPROM.h>


uint8_t Settings::readSetting(int addr)
{
    return EEPROM.read(addr);
}

void Settings::writeSetting(int addr, uint8_t value)
{
    if (value < UNREADVALUE)
    {
        EEPROM.write(addr, value);
      ///  EEPROM.commit();
    }
}

bool Settings::init(const size_t sz)
{
    //return EEPROM.begin(sz);
    return true;
};

uint8_t Settings::initSetting(int addr, int def)
{

    uint8_t setting = readSetting(addr);
    if (setting == UNREADVALUE)
    {
        setting = def;
        writeSetting(addr, setting);
    }
    return setting;
}