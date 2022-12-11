#ifndef OBSIDIAN_PROJECT_RESOURCE
#define OBSIDIAN_PROJECT_RESOURCE

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include "Media.hpp"
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

struct ResourceOutput {
    std::map<uint64_t, MediaBank> videoTracks;
    std::map<uint64_t, MediaBank> audioTracks;
};

class Resource {
public:
    Resource(const std::string& name, MediaBin& bin);
    virtual ~Resource();

    void read(const ResourceConfig& config, ResourceOutput& bank);
private:
    rational ts2q(int64_t pts, int index);
    void seek(int64_t frame, rational fps);
    void batch(const ResourceConfig& config, ResourceOutput& bank);
    MediaCache* cache(AVPacket *packet, AVFrame *frame);

    AVFormatContext *m_context;

    std::vector<ResourceStream> m_tracks;
    std::vector<int> m_vtracks; //Visual tracks
    std::vector<int> m_atracks; //Audial tracks
    std::vector<int> m_utracks; //Unknown tracks

    bool batched();
    /* Reason for supposed duplication of cache bank is for appropriate unlinking */
    Media *m_media;
};

#endif
