#include "Media.hpp"

Media::Media(uint32_t indices) {
	streams = indices;
	cache = std::vector<MediaBank>(indices);
	for(uint32_t i = 0; i < indices; ++i)
		cache[i] = MediaBank();
}

Media::~Media() {
	for (auto c : cache)
		c.clear();
	cache.clear();
}

/* This get assumes that the media cache about to be retrieved 
 * does not exist in the context's cache bank, and thus will increase
 * the number of references. Thus, contexts should ensure the frame
 * they need is already in their bank before calling this again to
 * prevent memory leaks from happening.
 */
std::shared_ptr<MediaCache> Media::get(uint32_t index, rational time) {
	if (index >= streams)
		return nullptr;
	// Prevents race conditioning and basic counting problems
	mutex.lock();
	for (std::shared_ptr<MediaCache>& c : cache[index]) {
		// It is an closed-open interval to prevent ambiguity
		if (c->start <= time && time < c->end) {
			mutex.unlock();
			return c;
		}
	}
	mutex.unlock();
	return nullptr;
}

/* If a duplication of this media cache exists, then this one
 * will be deleted, and you will be given the copy that already
 * exists instead. Use get before ever creating a cache
 */
std::shared_ptr<MediaCache> Media::link(uint32_t index, MediaCache *media) {
	if (index >= streams) {
		delete media;
		return nullptr;
	}
	// Prevents race conditioning and basic counting problems
	mutex.lock();

	for (std::shared_ptr<MediaCache>& c : cache[index]) {
		/* Since everything else between shared contexts should be
		 * equal, the only things to check are the time spans. If two
		 * contexts can create different kinds of information in the
		 * same stream index, we have a problem.
		*/
		if (c->start == media->start && c->end == media->end) {
			delete media;
			mutex.unlock();
			return c;
		}
	}
	std::shared_ptr<MediaCache> mc(media);
	cache[index].push_back(mc);
	mutex.unlock();
	return mc;
}

/* Unlinks all caches to clear up space, if only this instance owns it
 * it is cleaned up and removed from the list
 */
void Media::unlink() {
	mutex.lock();
	for (MediaBank& stream : cache) {
		for (size_t i = 0; i < stream.size(); ++i) {
			if (stream[i].unique())
				stream.erase(stream.begin() + (i--));
		}
	}
	mutex.unlock();
	
}