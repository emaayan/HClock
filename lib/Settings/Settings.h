#ifndef D75F8919_7B4A_4587_982E_E0A3D162DF92
#define D75F8919_7B4A_4587_982E_E0A3D162DF92

#include <stdint.h>
const int UNREADVALUE = 255;

uint8_t readSetting(int addr);
void writeSetting(int addr, uint8_t value);
uint8_t initSetting(int addr, int def);
#endif /* D75F8919_7B4A_4587_982E_E0A3D162DF92 */
