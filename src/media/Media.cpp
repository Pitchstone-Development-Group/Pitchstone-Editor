#include "Media.hpp"
#include <filesystem>

Media::Media(uint32_t indices) {
    streams = indices;
    cache = new std::vector<MediaCache*>[indices];
    for(uint32_t i = 0; i < indices; ++i)
        cache[i] = std::vector<MediaCache*>();
}

Media::~Media() {
    for (size_t i = 0; i < streams; ++i) {
        for (MediaCache *mc : cache[i])
            clean(mc);
        cache[i].clear();
    }
    delete[] cache;
}

/* This get assumes that the media cache about to be retrieved 
 * does not exist in the context's cache bank, and thus will increase
 * the number of references. Thus, contexts should ensure the frame
 * they need is already in their bank before calling this again to
 * prevent memory leaks from happening.
 */
MediaCache* Media::get(uint32_t index, float time) {
    if (index >= streams)
        return nullptr;
    // Prevents race conditioning and basic counting problems
    mutex.lock();

    MediaCache *media = nullptr;

    for (MediaCache *mc : cache[index]) {
        // It is an closed-open interval to prevent ambiguity
        if (mc->start <= time && time < mc->end) {
            media = mc;
            media->refs++;
            break;
        }
    }
    mutex.unlock();
    return media;
}

/* If a duplication of this media cache exists, then this one
 * will be deleted, and you will be given the copy that already
 * exists instead. Use get before ever creating a cache
 */
MediaCache* Media::link(uint32_t index, MediaCache *media) {
    if (index >= streams)
        return media;
    // Prevents race conditioning and basic counting problems
    mutex.lock();

    for (MediaCache *mc : cache[index]) {
        /* Since everything else between shared contexts should be
         * equal, the only things to check are the time spans. If two
         * contexts can create different kinds of information in the
         * same stream index, we have a problem.
        */
        if (mc->start == media->start && mc->end == media->end) {
            clean(media);
            media = mc;
            goto done;
        }
    }
    cache[index].push_back(media);
done:
    mutex.unlock();
    return media;
}

/* Unlinks all caches before and after a certain time to clear up
 * space, if any cache's references hit zero, it is cleaned up and
 * removed from the list
 */
uint32_t Media::unlink(uint32_t index, float before, float after) {
    if (index >= streams)
        return 0;
    mutex.lock();
    uint32_t unlinked = 0;
    for (size_t i = 0; i < cache[index].size(); ++i) {
        MediaCache *mc = cache[index][i];
        if (mc->end < before || after <= mc->start) {
            cache[index][i]->refs--;
            if (cache[index][i] == 0) {
                clean(mc);
                unlinked++;
                cache[index].erase(cache[index].begin() + (i--));
            }
        }
    }
    mutex.unlock();
    return unlinked;
}

int Media::unlink(uint32_t index, MediaCache* media) {
     if (index >= streams)
        return -2;
    int ret = -1;
    mutex.lock();
    for (size_t i = 0; i < cache[index].size(); ++i) {
        MediaCache *mc = cache[index][i];
        // Since copies are not allowed, this strangely works
        if (mc == media) {
            ret = 0;
            cache[index][i]->refs--;
            if (cache[index][i] == 0) {
                clean(mc);
                ret = 1;
                cache[index].erase(cache[index].begin() + i);
                break;
            }
        }
    }
    mutex.unlock();
    return ret;
    
}

static bool is_av(MediaCache *mc) {
    return mc->format >= MC_AV_VISUAL && mc->format <= MC_AV_OTHER;
}

void Media::clean(MediaCache *mc) {
    if (is_av(mc)) {
        av_frame_free(&mc->u.av);
    } else {
        for (int i = 0; i < MC_DATA_SIZE; ++i) {
            if (mc->u.other.data[i])
                delete[] mc->u.other.data[i];
            mc->u.other.size[i] = 0;
        }
        for (int i = 0; i < MC_META_SIZE; ++i)
            mc->u.other.meta[i] = 0;
    }
    mc->format = MC_INVALID;
    mc->start = mc->end = 0;
}


MediaBin::MediaBin() {
    bin = std::unordered_map<std::string, Media*>();
}

MediaBin::~MediaBin() {
    mutex.lock();
    for (auto kv : bin)
        delete kv.second;
    bin.clear();
    mutex.unlock();
}

Media* MediaBin::link(std::string fname, uint32_t streams) {
    std::filesystem::path p = std::filesystem::absolute(fname);
    mutex.lock();
    std::pair<std::string, Media*> pair;
    Media *media = nullptr;
    for (auto kv : bin) {
        if (kv.first == p) {
            media = kv.second;
            goto done;
        }
    }
    media = new Media(streams);

    pair.first = p.string(), pair.second = media;
    bin.insert(pair);
done:
    mutex.unlock();
    return media;
}