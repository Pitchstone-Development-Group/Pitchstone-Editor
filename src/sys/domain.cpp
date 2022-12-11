#include "domain.hpp"

domain::domain() {
    domains.clear();
}

domain::~domain() {
    domains.clear();
}

bool domain::intersects(rational a1, rational b1, rational a2, rational b2) {
    if (b1 == a2 || a1 == b2)
        return true;
    if (a2 >= a1 && b1 >= a2)
        return true;
    if (a1 >= a2 && b2 >= a1)
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
        if (intersects(domains[i].first, domains[i].second, a, b)) {
            domains[i].first = min(domains[i].first, a);
            domains[i].second = max(domains[i].second, b);
            for (size_t j = i + 1; j < domains.size(); ++j) {
                if (!intersects(domains[i].first, domains[i].second, domains[j].first, domains[j].second))
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