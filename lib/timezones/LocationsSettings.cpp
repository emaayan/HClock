#include <LocationsSettings.hpp>


// const offset_t INIT_OFFSET = 0;
// const offset_t COUNTRY_OFFSET = INIT_OFFSET + 4;
// const offset_t CITY_OFFSET = COUNTRY_OFFSET + 4;


// const EepromPosition START=400;
// const size_t RESERVE=512; //CITY_OFFSET+100;//UPDATE THIS ANYTIME, WHEN ADD NEW SETTING

LocationsSettings::LocationsSettings():_settings("Settings")
{

}
u_int32_t _initConfig = -1;

bool LocationsSettings::isStarted()
{
    return true;
}
u_int32_t LocationsSettings::loadInit()
{
    _initConfig = _settings.load("initConfig",_initConfig);// settings.load(INIT_OFFSET);
    return _initConfig;
}

void LocationsSettings::saveInit()
{
    _settings.save("initConfig", _initConfig);    
}

void LocationsSettings::loadCountry()
{
    char value[COUNTRY_CODE_SIZE] = "";
    _settings.load("Country", value, COUNTRY_CODE_SIZE,"");
    locations.setCurrentCountry(reinterpret_cast<char *>(value));
}

// must not be called in fnInvoke

char *LocationsSettings::saveCountry()
{
    char *value =locations.getCurrentCountry();
    _settings.save("Country", value);
    return locations.getCurrentCountry();
}



cityCode LocationsSettings::saveCity()
{
    if (locations.hasCity())
    {
        const City &ct = locations.getCurrentCity();
        _settings.save("City", ct.id);
        return ct.id;
    }
    else
    {
        return locations.getCurrentCity().id;
    }
}

cityCode LocationsSettings::loadCity()
{
    cityCode cc = _settings.load("City",CitiesDAO::NULL_CITY.id);
    locations.setCurrentCity(cc);
    return locations.getCurrentCity().id;
}
LocationsSettings locationSettings;