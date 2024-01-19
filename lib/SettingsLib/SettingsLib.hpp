#ifndef C33F6B3B_70DE_40D8_AAE0_677996B2096F
#define C33F6B3B_70DE_40D8_AAE0_677996B2096F

#include <Preferences.h>

class SettingsLib
{
public:
    explicit SettingsLib(const char *name) : _name(name)
    {
    }

   

    bool save(const char *key, const u_int32_t value)
    {
        return invoke(_name, [=]()
                      { pref.putULong(key, value);
              return true; });
    }
  
    u_int32_t load(const char *key, const u_int32_t def)
    {

        u_int32_t value = 0;
        if (invoke(_name, [&]()
                   {
                    value=pref.getULong(key, def);             
                      return true; }))
        {
            return value;
        }
        else
        {
            return def;
        }

        return value;
    };

    bool save(const char *key, const char *value)
    {
        return invoke(_name, [=]()
                      { pref.putString(key,value);
              return true; });
    }
    const char *load(const char *key, char *value, const size_t sz, const char *def)
    {
        if (invoke(_name, [&]()
                   {
                  size_t ret= pref.getString(key,value, sz);             
                      return ret>0; }))
        {
            return value;
        }
        else
        {
            return def;
        }
    }

private:
    template <typename Callback>
    bool invoke(const char *name, Callback callback)
    {
        if (pref.begin(name))
        {
            bool v = callback();
            pref.end();
            return v;
        }
        else
        {
            return false;
        }
    }

    const char *_name;
    Preferences pref;
};

#endif /* C33F6B3B_70DE_40D8_AAE0_677996B2096F */
