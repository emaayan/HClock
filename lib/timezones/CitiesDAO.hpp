#ifndef BFB3DC3A_4FD5_43EE_AF46_8BC81089EE50
#define BFB3DC3A_4FD5_43EE_AF46_8BC81089EE50

#include <stddef.h>
#include <stdint.h>
#include <vector>

typedef int32_t cityCode;
struct City//TODO: make this immutable and passed by reference (as well as country)
{
     cityCode id;
     char name[35];
     double lat;
     double lng;
     double elevation;
     char dstRules[35];
};

class CitiesDAO
{

public:
    static const City NULL_CITY;
    std::vector<City> &getCities();

    bool isNull(const City &city);
    size_t getSize();
    const City& get(int row);        
    void clear();
    void add(const City &city);
    
    const City &findBy(const cityCode ctCode);

private:
    std::vector<City> vCities;
} ;

extern CitiesDAO citiesDao;


#endif /* BFB3DC3A_4FD5_43EE_AF46_8BC81089EE50 */
