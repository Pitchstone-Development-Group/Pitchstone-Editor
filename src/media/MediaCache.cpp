#include "MediaCache.hpp"

MediaCache::MediaCache() {
	format = MC_INVALID;
	start = INT32_MIN, end = INT32_MAX;
	u.av = nullptr;
	for (size_t i = 0; i < metaDataSize; ++i)
		u.other.meta[i] = 0;
	for (size_t i = 0; i < dataSize; ++i) {
		u.other.data[i] = nullptr;
		u.other.size[i] = 0;
	}
}

MediaCache::MediaCache(AVMediaType type, AVRational time_base, int64_t pts, int64_t duration, AVFrame *frame) {
	switch(type) {
		case AVMEDIA_TYPE_VIDEO:
			format = MC_AV_VISUAL;
			break;
		case AVMEDIA_TYPE_AUDIO:
			format = MC_AV_AUDIAL;
			break;
		default:
			format = MC_AV_OTHER;
	}
	start = ~(rational(time_base) * pts), end = ~(rational(time_base) * (pts + duration));
	u.av = frame;
}

MediaCache::~MediaCache() {
	if (isAv())
		av_frame_free(&u.av);
	else {
		for (size_t i = 0; i < metaDataSize; ++i)
		u.other.meta[i] = 0;
		for (size_t i = 0; i < dataSize; ++i) {
			delete[] u.other.data[i];
			u.other.data[i] = nullptr;
			u.other.size[i] = 0;
		}
	}
	format = MC_INVALID;
}