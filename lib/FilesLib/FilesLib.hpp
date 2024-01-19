#ifndef D0DAB63E_E20A_409F_9AEF_B81A51860A12
#define D0DAB63E_E20A_409F_9AEF_B81A51860A12
#include "FS.h"
#include <LittleFS.h>
struct status
{
    int code = 0;
    char error[30] = "";
};

bool initFS();

template <typename Callback>
status processFile(const char *fileName, Callback fileHandler)
 //status processFile(const char *fileName, const std::function<status(File)> &fileHandler)
{
    status ret;
    File file = LittleFS.open(fileName);
    if (!file)
    {
        ret.code = -1;
        snprintf(ret.error, sizeof(ret.error), "Faild to open file %s ", fileName);
    }
    else
    {
        ret = fileHandler(file);
        file.close();
    }
    return ret;
}

#endif /* D0DAB63E_E20A_409F_9AEF_B81A51860A12 */
