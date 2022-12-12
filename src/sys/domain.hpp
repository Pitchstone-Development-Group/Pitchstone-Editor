#ifndef PITCHSTONE_UTIL_DOMAIN
#define PITCHSTONE_UTIL_DOMAIN

#include <vector>
#include <utility>
#include "rational.hpp"

class domain {
public:
    domain();
	domain(const domain& d) { domains = d.domains; }
    virtual ~domain();

	domain operator=(const domain& d) { domains = d.domains; return *this; }
	friend std::ostream& operator<<(std::ostream& os, const domain& d) { os << "{ "; for (auto r : d.domains) os << "[" << r.first << "," << r.second << ")"; os << " }"; return os; }
    
    inline void clear() { domains.clear(); }
    //Inserts continuous block [a,b)
    void insert(rational a, rational b);
    //Check if domains list contains a superdomain of [a,b)
    bool has(rational a, rational b);

    static bool intersects(rational a1, rational b1, rational a2, rational b2);
    static bool overlaps(rational a1, rational b1, rational a2, rational b2);
//private:
    std::vector<std::pair<rational, rational>> domains;
};

#endif
