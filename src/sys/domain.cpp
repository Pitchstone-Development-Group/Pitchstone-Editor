#include "domain.hpp"

domain::domain() {

}

domain::~domain() {

}

bool intersects(std::pair<rational, rational> p, rational a, rational b);
bool intersects(std::pair<rational, rational> p, rational a, rational b) {
    if (p.second == a || p.first == b)
        return true;
    if (a >= p.first && p.second >= a)
        return true;
    if (p.first >= a && b >= p.first)
        return true;
    return false;
}

#define min(a,b) ((a < b) ? a : b)
#define max(a,b) ((a > b) ? a : b)

void domain::insert(rational a, rational b) {
    //Handle basic cases 
    if (!domains.size() || b < domains[0].first) {
        domains.insert(domains.begin(), {a,b});
        return;
    } else if (a > domains[domains.size() - 1].second) {
        domains.push_back({a,b});
        return;
    }

    for (size_t i = 0; i < domains.size(); ++i) {
        //This implies intersections don't exist and can escape
        if (b < domains[i].first) {
            domains.insert(domains.begin(), {a,b});
            return;
        }
        //Because we guarentee ourselves a sorted list, we only need to start merging from here
        if (intersects(domains[i], a, b)) {
            domains[i].first = min(domains[i].first, a);
            domains[i].second = max(domains[i].second, b);
            for (size_t j = i + 1; j < domains.size(); ++j) {
                if (!intersects(domains[i], domains[j].first, domains[j].second))
                    break;
                domains[i].first = min(domains[i].first, domains[j].first);
                domains[i].second = max(domains[i].second, domains[j].second);
                domains.erase(domains.begin() + (int64_t) (j--));
            }
            return;
        }
    }

}

#undef min
#undef max

bool domain::has(rational a, rational b) {
    for (auto d : domains)
        if (a <= d.first && d.second <= b)
            return true;
    return false;
}