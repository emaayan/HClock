#include <Locations.hpp>

Locations locations;
bool Locations::hasCity()
{
    return !citiesDao.isNull(_city);
}

City &Locations::getCurrentCity()
{
    return _city;
}

bool Locations::clearCity()
{
    return setCurrentCity(CitiesDAO::NULL_CITY.id);
}

bool Locations::setCurrentCity(const cityCode ctCode)
{
    const City &ct = getCurrentCity();
    if (ctCode != ct.id)
    {
        if (ctCode != CitiesDAO::NULL_CITY.id)
        {
            const City &tempCity = citiesDao.findBy(ctCode);
            if (!citiesDao.isNull(tempCity))
            {
                _city = tempCity;
            }
            else
            {
                _city = citiesDao.get(0);
            }
        }
        else
        {
            _city = CitiesDAO::NULL_CITY;
        }
        return true;
    }
    else
    {
        return false;
    }
}
bool Locations::hasCountry()
{
    return strnlen(_countryCode, sizeof(_countryCode)) > 0;
}

char *Locations::getCurrentCountry()
{
    return _countryCode;
}

// WARNINGS EEPROM relies on this on save!!!
bool Locations::setCurrentCountry(const char countryCode[COUNTRY_CODE_SIZE]) // cant' have notifications here if you load something into memory that will alose cause notifciations
{

    if (strncmp(_countryCode, countryCode, sizeof(_countryCode)))
    {
        const Country &ctr = countriesDAO.findBy(countryCode);
        Country selectedCountry;
        if (!countriesDAO.isNull(ctr))
        {
            selectedCountry = ctr;
        }
        else
        {
            selectedCountry = countriesDAO.get(0);
        }
        const char *v = strncpy(_countryCode, selectedCountry.code, sizeof(_countryCode));
        clearCity();
        loadCityBy(v);
        if (citiesDao.getSize() > 0)
        {
            const City &ct = citiesDao.get(0);
            setCurrentCity(ct.id);
        }
        return true;
    }
    else
    {
        return false;
    }
}
