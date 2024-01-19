#include <LocationsDAO.hpp>
#include <CitiesDAO.hpp>
#include <CountriesDAO.hpp>

template <typename T>
size_t fillParts(DynamicJsonDocument &doc, std::vector<T> &v, const std::function<T(JsonObject)> &toObj)
{
    JsonArray arr = doc.as<JsonArray>();
    v.clear();
    for (JsonObject jso : doc.as<JsonArray>())
    {
        const T t = toObj(jso);
        v.push_back(t);
    }
    return v.size();
}

const Country toCountry(const JsonObject jso)
{
    Country ctr;
    strlcpy(ctr.code, jso["countryCode"] | "", sizeof(ctr.code));
    strlcpy(ctr.name, jso["country"] | "", sizeof(ctr.name));
    ctr.isIsrael=jso["israel"];
    return ctr;
};

const City toCity(const JsonObject jso)
{
    City city;
    city.id = jso["id"];
    strlcpy(city.name, jso["name"] | "", sizeof(city.name));
    city.lat = jso["lat"];
    city.lng = jso["lng"];
    city.elevation = jso["elevation"];
    strlcpy(city.dstRules, jso["dstRules"] | "", sizeof(city.dstRules));

    return city;
};

DynamicJsonDocument citiesJson(35000);
status loadCityBy(const char *countryCode)
{
    char fCityFilename[20] = {'\0'};
    snprintf(fCityFilename, sizeof(fCityFilename), "/loc/%s.json", countryCode);
    status status = loadJson(fCityFilename, citiesJson);
    if (status.code >= 0)
    {
        std::vector<City> &vCities = citiesDao.getCities();
        status.code = fillParts<City>(citiesJson, vCities, toCity);
        citiesJson.clear();
        std::sort(vCities.begin(), vCities.end(), [](const City &a, const City &b)
                  { return strncmp(a.name, b.name, 5) < 0; });
    }
    return status;
}

DynamicJsonDocument countriesJson(13000);
status loadCountries()
{

    status sta;
    if (countriesDAO.getSize() == 0)
    {
        // StaticJsonDocument countriesJson();
        const char countryFileName[] = "/loc/countries.json";
        sta = loadJson(countryFileName, countriesJson);
        if (sta.code == 0)
        {
            std::vector<Country> &vCountries = countriesDAO.getCountries();
            sta.code = fillParts<Country>(countriesJson, vCountries, toCountry);

            countriesJson.clear();
            std::sort(vCountries.begin(), vCountries.end(), [](const Country &a, const Country &b)
                      { return strncmp(a.name, b.name, 5) < 0; });
        }
    }
    else
    {
        sta.code = countriesDAO.getSize();
    }
    if (sta.code <= 0)
    {
        snprintf(sta.error, sizeof(sta.error), "No countries %s", "");
    }
    return sta;
}
