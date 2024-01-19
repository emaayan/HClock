
#include <JsonUtils.hpp>

status loadJson(const char *fileName, DynamicJsonDocument &doc)
{
    status sta;
    sta = processFile(fileName, [&](File file)
                      {
        status ret;            
        doc.clear();
        DeserializationError errorJson = deserializeJson(doc, file);
        ret.code = errorJson ? -1 : 0;
        snprintf(ret.error, sizeof(ret.error), "%s", errorJson.c_str()); 
        return ret; });
    return sta;
}