#include "Resource.hpp"
#include <iostream>

/* TODO: Write configuration for wrtie mode for this section */
Resource::Resource(const std::string& name, MediaBin& bin) {
    int ret;
    const AVCodec *codec;
    ResourceStream stream;

    m_context = NULL;
    ret = avformat_open_input(&m_context, name.c_str(), NULL, NULL);
    if (ret < 0)
        goto failure;

    ret = avformat_find_stream_info(m_context, NULL);
    if (ret < 0 || m_context->nb_streams == 0)
        goto failure;

    for (int i = 0; i < m_context->nb_streams; ++i) {
        
        codec = avcodec_find_decoder(m_context->streams[i]->codecpar->codec_id);
        if (codec == NULL)
            goto failure;
        
        stream.stream = m_context->streams[i];
        stream.context = avcodec_alloc_context3(codec);
        stream.temp.batched = false;
        stream.cache = MediaBank();
        stream.lstart = stream.lend = -1;
        avcodec_parameters_to_context(stream.context, stream.stream->codecpar);

        ret = avcodec_open2(stream.context, codec, NULL);
        if (ret < 0)
            goto failure;

        m_tracks.push_back(stream);

        switch (codec->type) {
            case AVMEDIA_TYPE_VIDEO:
                m_vtracks.push_back(i);
                break;
            case AVMEDIA_TYPE_AUDIO:
                m_atracks.push_back(i);
                break;
            default:
                m_utracks.push_back(i);
        }
    }
    m_media = bin.link(name, m_tracks.size());
    return;

failure:  
    for (ResourceStream rs : m_tracks)
        avcodec_free_context(&rs.context);

    avformat_close_input(&m_context);
}

 /* TODO: Handle non AV cases for image importing */
void Resource::read(const ResourceConfig& config, MediaBank& bank) {
    //Because this media bank is read only from the engine, it can be safely discarded without proper unlinking
    size_t ntracks = config.countAudioTracks + config.countVideoTracks;

    int64_t tracks = assembleTracks();

    bank.clear();
    
    rational start(config.frame * config.framesPerSecond.n, config.framesPerSecond.d);
    rational end(config.frame * config.framesPerSecond.n + 1, config.framesPerSecond.d);

    for (ResourceStream rs : m_tracks) {
        rs.temp.batched = false;
        rs.temp.cells = domain();
        for (MediaCache *mc : rs.cache)
            if (domain::intersects(mc->start, mc->end, start, end)) {
                bank.push_back(mc);
                rs.temp.cells.insert(mc->start, mc->end);
            }
    }

    bool missingFrame = false;
    for (ResourceStream rs : m_tracks)
        if (!rs.temp.cells.has(start, end)) {
            missingFrame = true;
            break;
        }
    
    // Seek if needed, then grab frames in batches
    if (missingFrame) {
        if (config.seeked || config.direction == RESOURCE_PLAY_REVERSE)
            seek(config.frame, config.framesPerSecond);

        for (ResourceStream rs : m_tracks) {
            for (int i = 0; i < config.countAudioTracks && )
        }

        

        batch(config.frame, config.framesPerSecond, bank);
    }

    for (ResourceStream rs : m_tracks)
        if (config.direction == RESOURCE_PLAY_FORWARD)
            m_media->unlink(rs.stream->index, rational(INT64_MIN), start);
        else
            m_media->unlink(rs.stream->index, end, rational(INT64_MAX));
            
}

void Resource::seek(int64_t frame, rational fps) {
    //First stream is usually rendered before all other streams of similar time
    int64_t timestamp = (int64_t) ((frame * fps.n * m_context->streams[0]->time_base.den) / (fps.d * m_context->streams[0]->time_base.num));
    int ret = av_seek_frame(m_context, 0, timestamp, AVSEEK_FLAG_BACKWARD);
    if (ret < 0)
        std::cerr << "Frame seek failed: " << frame << ", " << fps << std::endl;
}

void Resource::batch(const ResourceConfig& config, ResourceOutput& output) {
    AVPacket packet;
    MediaCache *mc;
    int ret;

    rational time = config.framesPerSecond * (int64_t)config.frame;

    do {
        ret = av_read_frame(m_context, &packet);
        if (ret == AVERROR_EOF)
            break;
        if ((mc = cache(&packet, nullptr)) != nullptr);

        int index;
        AVFrame *frame;
        do {
            ret = avcodec_send_packet(m_tracks[index].context, &packet);
            ret = avcodec_receive_frame(m_tracks[index].context, frame);
        } while (ret == AVERROR(EAGAIN));

        rational start = ts2q(packet.pts, packet.stream_index);
        rational end = ts2q(packet.pts + packet.duration, packet.stream_index);

        if (ts2q(packet.pts, packet.stream_index) <= time && time < ts2q(packet.pts, packet.stream_index))

        
        goto done;
     } while (batched());
done:
    av_packet_unref(&packet);
}

rational Resource::ts2q(int64_t pts, int index) {
    return rational(m_tracks[index].stream->time_base) * pts;
}

/* If not null, it will simply see if a respected cache exists
 * Otherwise create some cache and insert it into the cache system
*/
MediaCache* Resource::cache(AVPacket *packet, AVFrame *frame) {
    rational start = ts2q(packet->pts, packet->stream_index);
    MediaCache *mc;
    if (!frame) {
        mc = m_media->get(packet->stream_index, start);
        if (mc)
            m_tracks[packet->stream_index].cache.push_back(mc);
        return mc;
    }
    rational end = ts2q(packet->pts + packet->duration, packet->stream_index);

    mc = new MediaCache();
    mc->start = start, mc->end = end;

    switch(m_tracks[packet->stream_index].stream->codecpar->codec_type) {
        case AVMEDIA_TYPE_VIDEO:
            mc->format = MC_AV_VISUAL;
        case AVMEDIA_TYPE_AUDIO:
            mc->format = MC_AV_AUDIAL;
        default:
            mc->format = MC_AV_OTHER;
    }

    mc->refs = 1;
    mc->u.av = frame;

    m_media->link(packet->stream_index, mc);
}

Resource::~Resource() {
    for (ResourceStream rs : m_tracks)
        avcodec_free_context(&rs.context);

    avformat_close_input(&m_context);
}

bool Resource::batched() {
    for (ResourceStream rs : m_tracks)
        if (!rs.temp.batched)
            return false;
    return true;
}
