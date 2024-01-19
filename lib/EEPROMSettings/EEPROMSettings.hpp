#ifndef B84EE0D5_F175_4690_8ED3_DEA6D53E27A7
#define B84EE0D5_F175_4690_8ED3_DEA6D53E27A7

#include <ArduinoEEPROMAbstraction.h>


typedef u_int8_t offset_t;

class EEPROMSettings
{
public:
    explicit EEPROMSettings(size_t size,EepromPosition start);
    u_int32_t save(const offset_t offset, const u_int32_t value);
    u_int32_t load(const offset_t offset);
    void save(const offset_t offset, const unsigned char value[], size_t sz);
    void load(const offset_t offset, unsigned char value[], size_t sz);
    bool isStarted();
private:
    const EepromPosition romStart;// = 400;
    ArduinoEEPROMAbstraction eepromWrapper;
};
// extern EEPROMSettings settings;

#endif /* B84EE0D5_F175_4690_8ED3_DEA6D53E27A7 */
