#include <CitiesDAO.hpp>

#include <vector>
#include <algorithm>
CitiesDAO citiesDao;

const City CitiesDAO::NULL_CITY={-1, "", 0, 0, 0, ""};   

std::vector<City> &CitiesDAO::getCities()
{
    return vCities;
}

size_t CitiesDAO::getSize()
{
    return vCities.size();
}
bool CitiesDAO::isNull(const City &city)
{
    return city.id == NULL_CITY.id;
}
const City &CitiesDAO::get(int row) // TODO: need to fix, what happens if a city is cleared?
// https://www.learncpp.com/cpp-tutorial/move-constructors-and-move-assignment/
{
    if (row < getSize())
    {
        return vCities[row];
    }
    else
    {
        return NULL_CITY;
    }
}

void CitiesDAO::clear()
{
    vCities.clear(); // TODO: does not free memory, need to fix, and not bass
    vCities = std::vector<City>();
}

void CitiesDAO::add(const City &city)
{
    vCities.push_back(city);
}

const City &CitiesDAO::findBy(const cityCode ctCode)
{
    if (getSize() > 0)
    {
        
        std::vector<City>::iterator iter = std::find_if(vCities.begin(), vCities.end(), [=](const City &ct)
                                                        { return ct.id == ctCode; });
        if(iter!=vCities.end()){
            return iter[0];
        }else{
            return NULL_CITY; 
        }
        
    }
    else
    {
        return NULL_CITY;
    }
}
