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

/* TODO: Write configuration for wrtie mode for this section */
Resource::Resource(const std::string& name, MediaBin& bin) {
	try {
		int ret;
		const AVCodec *codec;
		ResourceStream stream;
		m_context = nullptr;
		ret = avformat_open_input(&m_context, name.c_str(), nullptr, nullptr);
		if (ret < 0)
			throw -1;

		ret = avformat_find_stream_info(m_context, nullptr);
		if (ret < 0 || m_context->nb_streams == 0)
			throw -1;

		for (size_t i = 0; i < m_context->nb_streams; ++i) {
			
			codec = avcodec_find_decoder(m_context->streams[i]->codecpar->codec_id);
			if (codec == nullptr)
				throw -1;
		
			stream.stream = m_context->streams[i];
			stream.context = avcodec_alloc_context3(codec);
			stream.temp.batched = false;
			stream.cache = MediaBank();
			stream.lstart = stream.lend = -1;
			avcodec_parameters_to_context(stream.context, stream.stream->codecpar);

			ret = avcodec_open2(stream.context, codec, nullptr);
			if (ret < 0)
				throw -1;

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
	} catch(int code) {
		for (ResourceStream rs : m_tracks) {
			avcodec_free_context(&rs.context);
		}
		avformat_close_input(&m_context);
	}
}

Resource::~Resource() {
	for (ResourceStream rs : m_tracks) {
		rs.cache.clear();
		rs.temp.cells.clear();
		if (rs.context != nullptr)
			avcodec_free_context(&rs.context);
	}
	if (m_context != nullptr)
		avformat_close_input(&m_context);
}

 /* TODO: Handle non AV cases for image importing */
void Resource::read(const ResourceConfig& config, ResourceOutput& banks) {
	for (size_t i = 0; i < config.countAudioTracks; ++i)
		banks.audioTracks[i].clear();
	for (size_t i = 0; i < config.countVideoTracks; ++i)
		banks.videoTracks[i].clear();
	
	rational start(config.frame * config.framesPerSecond.d, config.framesPerSecond.n);
	rational end(config.frame * config.framesPerSecond.d + 1, config.framesPerSecond.n);

	for (ResourceStream& rs : m_tracks)
		rs.temp.batched = true;

	ResourceScraperConfig scraper = {config.countAudioTracks, config.audioTracks, m_atracks, start, end};
	scrapeCache(scraper, banks.audioTracks);

	scraper.countTracks = config.countVideoTracks, scraper.tracks = config.videoTracks, scraper.rtracks = m_vtracks;
	scrapeCache(scraper, banks.audioTracks);
	
	if (!batched()) {
		/* TODO: Will need to optimize the batching method in the case not all frames are cached
		 * This is because when grabbing cache of frames already processed, the internal readers do not get updated
		 * So grabbing the frame in batches would end up needing to read may more than needed only to free it in the end
		 * This is guarenteed to work for the mean time, but will need to improve once the application evolves
		 */
		if (config.seeked || config.direction == RESOURCE_PLAY_REVERSE)
			seek(config.frame, config.framesPerSecond);
		batch(config, banks);
	}

	// Gets rid of uneeded cache, the media object will handle memory freeing of cache
	for (ResourceStream& rs : m_tracks) {
		for (size_t i = 0; i < rs.cache.size(); ++i) {
			bool remove = false;
			if (config.direction == RESOURCE_PLAY_FORWARD && rs.cache[i]->end <= start)
				remove = true;
			else if (config.direction == RESOURCE_PLAY_REVERSE && rs.cache[i]->start >= end)
				remove = true;

			if (remove)
				rs.cache.erase(rs.cache.begin() + (i--));
		}
	}
	m_media->unlink();
}

/* Search through all cache and grab frames and packets from intersecting time spans */
void Resource::scrapeCache(ResourceScraperConfig& config, std::map<uint64_t, MediaBank>& bank) {
	for (size_t i = 0; i < config.countTracks; ++i) {
		ResourceStream& rs = m_tracks[config.rtracks[config.tracks[i]]];
		rs.temp.cells.clear();
		for (std::shared_ptr<MediaCache>& mc : rs.cache)
			if (domain::overlaps(mc->start, mc->end, config.start, config.end)) {
				bank[i].push_back(mc);
				rs.temp.cells.insert(mc->start, mc->end);
			}
		rs.temp.batched = rs.temp.cells.has(config.start, config.end);
	}
}

/* Checks if all tracks have their needed tracks */
bool Resource::batched() {
	for (ResourceStream rs : m_tracks)
		if (!rs.temp.batched)
			return false;
	return true;
}

void Resource::seek(int64_t frame, rational fps) {
	//First stream is usually rendered before all other streams of similar time
	int64_t timestamp = (int64_t) ((frame * fps.d * m_context->streams[0]->time_base.den) / (fps.n * m_context->streams[0]->time_base.num));
	int ret = av_seek_frame(m_context, 0, timestamp, AVSEEK_FLAG_BACKWARD);
	if (ret < 0)
		std::cerr << "Frame seek failed: " << frame << ", " << fps << std::endl;
}

void Resource::batch(const ResourceConfig& config, ResourceOutput& output) {
	AVPacket packet;
	MediaCache *mc;
	int ret;

	ResourceScraperConfig scraper;
	scraper.start = rational(config.frame * config.framesPerSecond.d, config.framesPerSecond.n);
	scraper.end = rational(config.frame * config.framesPerSecond.d + 1, config.framesPerSecond.n);

	do {
		ret = av_read_frame(m_context, &packet);

		if (ret < 0) {
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
		// Incase no family of tracks listens to it, to prevent memory leaks
		av_packet_unref(&packet);
	 } while (!batched());
}

/* Decodes frame from packet and inserts it to media cache for that track for future use
 * Returns whether this was the right family of tracks for the packet
 */
bool Resource::renderPacket(ResourceScraperConfig& config, std::map<uint64_t, MediaBank>& bank, AVPacket* packet) {
	std::shared_ptr<MediaCache> mc;
	rational pstart = ts2q(packet->pts, packet->stream_index);
	rational pend = ts2q(packet->pts + packet->duration, packet->stream_index);
	int ret, index = packet->stream_index;
	AVFrame *frame;

	for (size_t i = 0; i < config.countTracks; ++i) {
		ResourceStream& rs = m_tracks[config.rtracks[config.tracks[i]]];
		if (rs.stream->index != packet->stream_index)
			continue;

		// Despite already checking our local cache, another thread could create the needed cache
		mc = m_media->get(packet->stream_index, pstart);
		if (mc == nullptr) {
			index = packet->stream_index;
			frame = av_frame_alloc();
			do {
				ret = avcodec_send_packet(m_tracks[index].context, packet);
				ret = avcodec_receive_frame(m_tracks[index].context, frame);
			} while (ret == AVERROR(EAGAIN));

			AVStream *stream = m_tracks[packet->stream_index].stream;

			MediaCache *cache = new MediaCache(stream->codecpar->codec_type, stream->time_base, packet->pts, packet->duration, frame);
			
			// Despite creation of packet, doesn't mean it wasn't created from another thread, grab that media cache if so
			mc = m_media->link(packet->stream_index, cache);
		}
		rs.cache.push_back(mc);
		rs.temp.cells.insert(pstart, pend);
		rs.temp.batched = rs.temp.cells.has(config.start, config.end);
		if (domain::overlaps(mc->start, mc->end, config.start, config.end))
			bank[i].push_back(mc);
		av_packet_unref(packet);
		return true;
	}
	// This means the packet did not belong to family of tracks
	return false;
}

/* Time stamp for given stream index to rational in seconds, in reduced form */
rational Resource::ts2q(int64_t pts, int index) {
	return ~(rational(m_tracks[index].stream->time_base) * pts);
}