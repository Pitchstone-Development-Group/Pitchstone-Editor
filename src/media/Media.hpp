#ifndef OBSIDIAN_PROJECT_MEDIA
#define OBSIDIAN_PROJECT_MEDIA

#include <utility>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <string>
#include <stdint.h>

extern "C" {
#include <libavcodec/avcodec.h>
}

enum MCFormat {
    MC_AV_VISUAL,
    MC_AV_AUDIAL,
    MC_AV_OTHER,
    MC_OTHER_VISUAL,
    MC_OTHER_AUDIAL,
    MC_OTHER_OTHER,
    MC_INVALID = -1
}; 

#define MC_META_SIZE 8
#define MC_DATA_SIZE 4

struct MediaCache {
    enum MCFormat format;
    float start, end;
    union {
        AVFrame *av;
        struct {
            uint32_t meta[MC_META_SIZE];
            uint8_t *data[MC_DATA_SIZE];
            size_t   size[MC_DATA_SIZE];
        } other;
    } u;
    uint32_t refs;
};

using MediaBank = std::vector<MediaCache*>;

struct Media {
public:
    Media(uint32_t indices);
    virtual ~Media();

    MediaCache* get(uint32_t index, float time);
    MediaCache* link(uint32_t index, MediaCache*);
    uint32_t unlink(uint32_t index, float before, float after);
    int unlink(uint32_t index, MediaCache *media);
private:
    void clean(MediaCache*);

    size_t streams;
    MediaBank *cache;
    std::string fname;
    std::mutex mutex;
};

class MediaBin {
public:
    MediaBin();
    virtual ~MediaBin();
    Media* link(std::string fname, uint32_t streams);
private:
    std::unordered_map<std::string, Media*> bin;
    std::mutex mutex;
};

#endif
