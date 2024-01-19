
#include <CountriesDAO.hpp>

CountriesDAO countriesDAO;
#include <string.h>
#include <algorithm>
const Country CountriesDAO::NULL_COUNTRY = {"", "", false, 0};
std::vector<Country> &CountriesDAO::getCountries()
{
    return vCountries;
}

size_t CountriesDAO::getSize()
{
    return vCountries.size();
}

bool CountriesDAO::isNull(const Country &country)
{
    return !strncmp(country.code, NULL_COUNTRY.code, COUNTRY_CODE_SIZE);
}
const Country &CountriesDAO::get(int row)
{
    if (row < getSize())
    {
        return vCountries[row];
    }
    else
    {
        return NULL_COUNTRY;
    }
}

void CountriesDAO::clear()
{
    vCountries.clear();
    vCountries = std::vector<Country>();
}

void CountriesDAO::add(const Country &country)
{
    vCountries.push_back(country);
}

const Country &CountriesDAO::findBy(const char code[COUNTRY_CODE_SIZE])
{
    if (getSize() > 0)
    {
        std::vector<Country>::iterator iter = std::find_if(vCountries.begin(), vCountries.end(), [=](const Country &ctr)
                                                           { return !strncmp(code, ctr.code, COUNTRY_CODE_SIZE); });
        if (iter != vCountries.end())
        {
            return iter[0];
        }
        else
        {
            return NULL_COUNTRY;
        }
    }
    else
    {
        return NULL_COUNTRY;
    }

    return NULL_COUNTRY;
}
