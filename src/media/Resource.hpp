#ifndef OBSIDIAN_PROJECT_RESOURCE
#define OBSIDIAN_PROJECT_RESOURCE

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include "MediaBin.hpp"
#include "../sys/domain.hpp"
#include <map>

#define RESOURCE_PLAY_FORWARD 0
#define RESOURCE_PLAY_REVERSE 1

enum PacketType {
	PACKET_VISUAL,
	PACKET_AUDIAL,
	PACKET_UNKOWN = -1
}; 

struct ResourceStream {
	ResourceStream();
	AVStream* stream;
	AVCodecContext *context;
	MediaBank cache;
	struct {
		bool batched; // For when to stop batch calling
		domain cells; //For audio and video frame collection before finishing
	} temp;
	rational lstart, lend; // Last rendered packet timestamps;
};

struct ResourceConfig {
	uint64_t frame;
	rational framesPerSecond;
	uint32_t direction;
	uint32_t seeked;
	size_t countVideoTracks;
	uint64_t *videoTracks;
	size_t countAudioTracks;
	uint64_t *audioTracks;
};

/* For scraping and inserting cache */
struct ResourceScraperConfig {
	size_t countTracks;
	uint64_t *tracks;
	std::vector<int> rtracks;
	rational start, end;
};

struct ResourceOutput {
	std::map<uint64_t, MediaBank> videoTracks;
	std::map<uint64_t, MediaBank> audioTracks;
};

class Resource {
public:
	Resource(const std::string& name, MediaBin& bin);
	virtual ~Resource();

	void read(const ResourceConfig& config, ResourceOutput& bank);

	size_t numAudioTracks() { return m_atracks.size(); }
	size_t numVideoTracks() { return m_vtracks.size(); }

private:
	void scrapeCache(ResourceScraperConfig& config, std::map<uint64_t, MediaBank>& bank);
	bool renderPacket(ResourceScraperConfig& config, std::map<uint64_t, MediaBank>& bank, AVPacket* packet);
	rational ts2q(int64_t pts, int index);
	void seek(int64_t frame, rational fps);
	void batch(const ResourceConfig& config, ResourceOutput& bank);
	bool batched();

	AVFormatContext *m_context;

	std::vector<ResourceStream> m_tracks;
	std::vector<int> m_vtracks; //Visual tracks
	std::vector<int> m_atracks; //Audial tracks
	std::vector<int> m_utracks; //Unknown tracks

	Media *m_media;
};

#endif
