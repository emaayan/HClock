#ifndef D75F8919_7B4A_4587_982E_E0A3D162DF92
#define D75F8919_7B4A_4587_982E_E0A3D162DF92

#include <stdint.h>
#include <stddef.h>

const int UNREADVALUE = 255;

class Settings
{
public:
    
    virtual bool init(const size_t sz);
    uint8_t readSetting(int addr);
    virtual void writeSetting(int addr, uint8_t value);
    uint8_t initSetting(int addr, int def);

private:
};
#endif /* D75F8919_7B4A_4587_982E_E0A3D162DF92 */
