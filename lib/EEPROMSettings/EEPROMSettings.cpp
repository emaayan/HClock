#include <EEPROMSettings.hpp>
#include <EEPROM.h>
static bool started = false;
EEPROMSettings::EEPROMSettings(size_t size,EepromPosition start) : eepromWrapper(&EEPROM),romStart(start)
{

    if (!started && EEPROM.begin(size))
    {
        started = true;
    }
}
bool EEPROMSettings::isStarted()
{
    return started;
}
u_int32_t EEPROMSettings::save(const offset_t offset, const u_int32_t value)
{
    eepromWrapper.write32(romStart + offset, value);
    EEPROM.commit();    
    return value;
}
u_int32_t EEPROMSettings::load(const offset_t offset)
{
    return eepromWrapper.read32(romStart + offset);
}

void EEPROMSettings::save(const offset_t offset, const unsigned char value[], size_t sz)
{
    eepromWrapper.writeArrayToRom(romStart + offset, value, sz);
    EEPROM.commit();
}

void EEPROMSettings::load(const offset_t offset, unsigned char value[], size_t sz)
{
    eepromWrapper.readIntoMemArray(value, offset, sz);
}
