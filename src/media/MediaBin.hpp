#pragma once

#include "Media.hpp"

class MediaBin {
public:
    MediaBin();
    virtual ~MediaBin();
    Media* link(std::string fname, uint32_t streams);
private:
    std::unordered_map<std::string, Media*> bin;
    std::mutex mutex;
};
