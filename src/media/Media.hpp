#ifndef OBSIDIAN_PROJECT_MEDIA
#define OBSIDIAN_PROJECT_MEDIA

#include <utility>
#include <mutex>
#include <unordered_map>
#include <string>
#include <stdint.h>
#include "MediaCache.hpp"

class Media {
public:
    Media(uint32_t indices);
    virtual ~Media();

    std::shared_ptr<MediaCache> get(uint32_t index, rational time);
    std::shared_ptr<MediaCache> link(uint32_t index, MediaCache*);
	void unlink();
private:

    size_t streams;
    std::vector<MediaBank> cache;
    std::string fname;
    std::mutex mutex;
};

#endif
