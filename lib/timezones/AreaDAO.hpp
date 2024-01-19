#ifndef B9A724A5_54C3_43C2_A528_9DF0B1B4115F
#define B9A724A5_54C3_43C2_A528_9DF0B1B4115F

#include <stddef.h>
#include <stdint.h>
#include <vector>
template<typename area,typename code>
class AreaDAO
{

public:
    // static const area NULL_CITY;
    std::vector<area> &getAll();

    bool isNull(const area &geo);
    size_t getSize();
    const area& get(int row);        
    void clear();
    void add(const area &geo);
    
    const area &findBy(const code code);

private:
    std::vector<area> vAreas;
} ;

#endif /* B9A724A5_54C3_43C2_A528_9DF0B1B4115F */
