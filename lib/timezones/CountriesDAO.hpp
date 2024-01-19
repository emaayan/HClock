#ifndef C07A732D_821D_4B47_8679_EDEDD07CDBA9
#define C07A732D_821D_4B47_8679_EDEDD07CDBA9

#include <stdint.h>
#include <stddef.h>
#include <vector>

const size_t COUNTRY_CODE_SIZE = 4;
struct Country
{
    char code[COUNTRY_CODE_SIZE];
    char name[35];
    bool isIsrael;
    uint8_t totalCities;
};



class CountriesDAO
{
public:
    static const Country NULL_COUNTRY;
    std::vector<Country> &getCountries();

    bool isNull(const Country &country);
    size_t getSize();
    const Country &get(int row);
    void clear();
    void add(const Country &country);

    const Country &findBy(const char *code);

private:
    std::vector<Country> vCountries;
};

extern CountriesDAO countriesDAO;
#endif /* C07A732D_821D_4B47_8679_EDEDD07CDBA9 */
