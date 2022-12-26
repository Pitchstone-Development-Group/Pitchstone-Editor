#include "MediaBin.hpp"
#include <filesystem>

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
			break;
		}
	}
	if (!media)
	media = new Media(streams);

	pair.first = p.string(), pair.second = media;
	bin.insert(pair);
done:
	mutex.unlock();
	return media;
}