#ifndef PITCHSTONE_UTIL_DOMAIN
#define PITCHSTONE_UTIL_DOMAIN

#include <vector>
#include <utility>
#include "rational.hpp"

class domain {
public:
    domain();
    virtual ~domain();
    
    inline void clear() { domains.clear(); }
    //Inserts continuous block [a,b)
    void insert(rational a, rational b);
    //Check if domains list contains a superdomain of [a,b)
    bool has(rational a, rational b);
//private:
    std::vector<std::pair<rational, rational>> domains;
};

#endif
