#pragma once

#include "../sys/rational.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
}
#include <vector>
#include <memory>

enum MCFormat {
    MC_AV_VISUAL,
    MC_AV_AUDIAL,
    MC_AV_OTHER,
    MC_OTHER_VISUAL,
    MC_OTHER_AUDIAL,
    MC_OTHER_OTHER,
    MC_INVALID = -1
};

class MediaCache {
public:
	MediaCache();
	MediaCache(AVMediaType type, AVRational time_base, int64_t pts, int64_t duration, AVFrame *frame);
	~MediaCache();
	inline bool isAv() { return ((format >= MC_AV_VISUAL) && (format <= MC_AV_OTHER)); }
	static const size_t metaDataSize = 8, dataSize = 4; 
    enum MCFormat format;
    rational start, end;
    union {
        AVFrame *av;
        struct {
            uint32_t meta[metaDataSize];
            uint8_t *data[dataSize];
            size_t   size[dataSize];
        } other;
    } u;
};

using MediaBank = std::vector<std::shared_ptr<MediaCache>>;
