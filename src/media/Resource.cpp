#include "Resource.hpp"
#include <iostream>

ResourceStream::ResourceStream() {
	stream = nullptr;
	context = nullptr;
	cache = MediaBank();
	temp.batched = false;
	temp.cells = domain();
	lstart = lend = 0;
}

ResourceStream::ResourceStream(const ResourceStream& rs) {
	stream = rs.stream;
	context = rs.context;
	cache = rs.cache;
	temp.batched = rs.temp.batched;
	temp.cells = rs.temp.cells;
	lstart = rs.lstart;
	lend = rs.lend;
}

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

	for (size_t i = 0; i < m_context->nb_streams; ++i) {
		
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

Resource::~Resource() {
	for (ResourceStream rs : m_tracks)
		avcodec_free_context(&rs.context);

	avformat_close_input(&m_context);
}

 /* TODO: Handle non AV cases for image importing */
void Resource::read(const ResourceConfig& config, ResourceOutput& banks) {
	for (size_t i = 0; i < config.countAudioTracks; ++i)
		banks.audioTracks[i].clear();
	for (size_t i = 0; i < config.countVideoTracks; ++i)
		banks.videoTracks[i].clear();
	
	rational start(config.frame * config.framesPerSecond.n, config.framesPerSecond.d);
	rational end(config.frame * config.framesPerSecond.n + 1, config.framesPerSecond.d);

	for (ResourceStream& rs : m_tracks)
		rs.temp.batched = true;

	ResourceScraperConfig scraper = {config.countAudioTracks, config.audioTracks, m_atracks, start, end};
	scrapeCache(scraper, banks.audioTracks);

	scraper.countTracks = config.countVideoTracks, scraper.tracks = config.videoTracks, scraper.rtracks = m_vtracks;
	scrapeCache(scraper, banks.audioTracks);
	
	// Seek if needed, then grab frames in batches
	if (!batched()) {
		if (config.seeked || config.direction == RESOURCE_PLAY_REVERSE)
			seek(config.frame, config.framesPerSecond);
		batch(config, banks);
	}

	rational cstart = (config.direction == RESOURCE_PLAY_FORWARD) ? rational(INT64_MIN) : start;
	rational cend = (config.direction == RESOURCE_PLAY_FORWARD) ? end : rational(INT64_MAX);

	for (ResourceStream rs : m_tracks)
		for (size_t i = 0; i < rs.cache.size(); ++i)
			if (domain::overlaps(cstart, cend, rs.cache[i]->start, rs.cache[i]->end)) {
				m_media->unlink((uint32_t)rs.stream->index, rs.cache[i]);
				delete rs.cache[i];
				rs.cache.erase(rs.cache.begin() + (i--));
			}
}

void Resource::scrapeCache(ResourceScraperConfig& config, std::map<uint64_t, MediaBank>& bank) {
	for (size_t i = 0; i < config.countTracks; ++i) {
		ResourceStream& rs = m_tracks[config.rtracks[config.tracks[i]]];
		rs.temp.cells.clear();
		for (MediaCache *mc : rs.cache)
			if (domain::overlaps(mc->start, mc->end, config.start, config.end)) {
				bank[i].push_back(mc);
				rs.temp.cells.insert(mc->start, mc->end);
			}
		rs.temp.batched = rs.temp.cells.has(config.start, config.end);
	}
}

bool Resource::batched() {
	for (ResourceStream rs : m_tracks)
		if (!rs.temp.batched)
			return false;
	return true;
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

	ResourceScraperConfig scraper;
	scraper.start = rational(config.frame * config.framesPerSecond.n, config.framesPerSecond.d);
	scraper.end = rational(config.frame * config.framesPerSecond.n + 1, config.framesPerSecond.d);

	do {
		ret = av_read_frame(m_context, &packet);
		if (ret == AVERROR_EOF)
			break;
		else if (ret < 0) {
			std::cerr << "AV batch packet read error: " << ret << std::endl;
			av_packet_unref(&packet);
			break;
		}

		scraper.countTracks = config.countAudioTracks, scraper.tracks = config.audioTracks, scraper.rtracks = m_atracks;
		if (renderPacket(scraper, output.audioTracks, &packet))
			continue;
		scraper.countTracks = config.countVideoTracks, scraper.tracks = config.videoTracks, scraper.rtracks = m_vtracks;
		if (renderPacket(scraper, output.videoTracks, &packet))
			continue;

	 } while (batched());
	
	av_packet_unref(&packet);
}

bool Resource::renderPacket(ResourceScraperConfig& config, std::map<uint64_t, MediaBank>& bank, AVPacket* packet) {
	MediaCache *mc;
	rational pstart = ts2q(packet->pts, packet->stream_index);
	rational pend = ts2q(packet->pts + packet->duration, packet->stream_index);

	for (size_t i = 0; i < config.countTracks; ++i) {
		ResourceStream& rs = m_tracks[config.rtracks[config.tracks[i]]];
		if (rs.stream->index != packet->stream_index)
			continue;

		mc = m_media->get(packet->stream_index, config.start);
		if (mc) {
			rs.cache.push_back(mc);
			rs.temp.cells.insert(pstart, pend);
			rs.temp.batched = rs.temp.cells.has(config.start, config.end);
			return true;
		}

		int ret, index = packet->stream_index;
		AVFrame *frame = av_frame_alloc();
		do {
			ret = avcodec_send_packet(m_tracks[index].context, packet);
			ret = avcodec_receive_frame(m_tracks[index].context, frame);
		} while (ret == AVERROR(EAGAIN));

		mc = new MediaCache();
		mc->start = pstart, mc->end = pend;
		mc->refs = 1;
		mc->u.av = frame;

		switch(m_tracks[packet->stream_index].stream->codecpar->codec_type) {
			case AVMEDIA_TYPE_VIDEO:
				mc->format = MC_AV_VISUAL;
				break;
			case AVMEDIA_TYPE_AUDIO:
				mc->format = MC_AV_AUDIAL;
				break;
			default:
				mc->format = MC_AV_OTHER;
		}

		m_media->link(packet->stream_index, mc);
		return true;
	}
	return false;
}
