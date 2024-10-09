#include <FilesLib.hpp>

bool initFS()
{
    return LittleFS.begin(true);
}
