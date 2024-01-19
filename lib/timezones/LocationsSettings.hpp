#ifndef B6777ECD_0243_4EBA_B402_B49D67FC7719
#define B6777ECD_0243_4EBA_B402_B49D67FC7719

// #include <settings.h>
#include <Locations.hpp>
// #include <EEPROMSettings.hpp>
#include <SettingsLib.hpp>
class LocationsSettings
{
public:
    
    LocationsSettings();
    bool isStarted();
    u_int32_t loadInit();
    void saveInit();

    void loadCountry();
    // must not be called in fnInvoke
    char *saveCountry();

    cityCode loadCity();
    // must not be called in fnInvoke
    cityCode saveCity();

private:
    SettingsLib _settings;//("Settings");
    // EEPROMSettings settings;
};
extern LocationsSettings locationSettings;
#endif /* B6777ECD_0243_4EBA_B402_B49D67FC7719 */
