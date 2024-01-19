#ifndef A43D950B_2080_4E1A_9714_FEA7F2D1669D
#define A43D950B_2080_4E1A_9714_FEA7F2D1669D

#include <stdint.h>
#include <stddef.h>
#include <LocationsDAO.hpp>

class Locations
{
public:
    bool hasCity();
    bool clearCity();
    City &getCurrentCity();
    bool setCurrentCity(const cityCode ctCode);

    bool hasCountry();
    char *getCurrentCountry();
    bool setCurrentCountry(const char *countryCode);

private:
    char _countryCode[COUNTRY_CODE_SIZE] =  {'\0'};
    City _city =CitiesDAO::NULL_CITY;
};
extern Locations locations;
#endif /* A43D950B_2080_4E1A_9714_FEA7F2D1669D */
