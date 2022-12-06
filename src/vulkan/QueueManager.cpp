#include "QueueManager.hpp"
#include <iostream>
#include <utility>
#include <vector>
#include <iostream>

/* Stores the support for present, graphics, and compute for each family as well as queue count and occupied */
struct qf_capabilities {
	int p, g, c, q, o;
	qf_capabilities() noexcept { p = g = c = q = o = 0; }
	qf_capabilities(int _p, int _g, int _c, int _q) { p = _p, g = _g, c = _c, q = _q, o = 0; }
	qf_capabilities(const qf_capabilities& k) noexcept { p = k.p, g = k.g, c = k.c, q = k.q, o = k.o; } 
	bool operator==(const qf_capabilities& k) { return p == k.p && g == k.g && c == k.c; }
};

struct qf_config {
	qf_config() { f = i = (uint32_t)-1; }
	qf_config(uint32_t fam, uint32_t ind) { f = fam, i = ind; }
	qf_config(const qf_config& q) { f = q.f, i = q.i; }
	qf_config operator=(const qf_config &q) { f = q.f, i = q.i; return *this; }
	bool operator==(const qf_config& q) { return f == q.f && i == q.i; }
	uint32_t f, i;
};

/* Order or preference when looking for window queue, compute queue, and the two graphics queues */
static std::vector<qf_capabilities> w_prefs({
	qf_capabilities(1, 1, 0, 0),
	qf_capabilities(1, 1, 1, 0),
});

static std::vector<qf_capabilities> c_prefs({
	qf_capabilities(0, 0, 1, 0),
	qf_capabilities(1, 0, 1, 0),
	qf_capabilities(0, 1, 1, 0),
	qf_capabilities(1, 1, 1, 0)
});

static std::vector<qf_capabilities> g_prefs({
	qf_capabilities(0, 1, 0, 0),
	qf_capabilities(0, 1, 1, 0),
	qf_capabilities(1, 1, 1, 0),
	qf_capabilities(1, 1, 0, 0)
});

#define NEG_1 ((uint32_t)-1)

/* Force the queue into the most capable queue family */
static qf_config pigeon_hole(const std::vector<qf_capabilities>& family, const std::vector<qf_capabilities>& prefs) {
	for (auto p : prefs)
		for (size_t i = 0; i < family.size(); ++i)
			if (p == family[i])
				return qf_config(i, family[i].q - 1);
	return qf_config();
}

/* Find the first family with a free queue that isn't in the same family as prior queues */
static qf_config find_first(std::vector<qf_capabilities>& family, const std::vector<qf_capabilities>& prefs, uint32_t wp, uint32_t wg, uint32_t c) {
	for (auto p : prefs)
		for (size_t i = 0; i < family.size(); ++i)
			if (p == family[i] && family[i].q > family[i].o && i != (size_t)wp && i != (size_t)wg && i != (size_t)c)
				return qf_config(i, family[i].o++);
	return qf_config();
}

static void solve(std::vector<qf_capabilities>& fam, qf_config *queues) {
	/* Set window present and grphic queues, attempting to combine them into one */
	queues[0] = find_first(fam, w_prefs, NEG_1, NEG_1, NEG_1);

	/* Compute queue is more important than the graphics queues and should be in its own family if it can */
	queues[1] = find_first(fam, c_prefs, queues[0].f, queues[1].f, NEG_1);
	if (queues[1].f == NEG_1)
		queues[1] = find_first(fam, c_prefs, NEG_1, NEG_1, NEG_1);
	if (queues[1].f == NEG_1)
		queues[1] = pigeon_hole(fam, c_prefs);
   
	/* Then the graphics queues get their share */
	queues[2] = find_first(fam, g_prefs, queues[0].f, queues[1].f, queues[2].f);
	if (queues[2].f == NEG_1)
		queues[2] = find_first(fam, g_prefs, queues[0].f, queues[1].f, NEG_1);
	if (queues[2].f == NEG_1)
		queues[2] = find_first(fam, g_prefs, NEG_1, NEG_1, NEG_1);
	if (queues[2].f == NEG_1)
		queues[2] = pigeon_hole(fam, g_prefs);

	queues[3] = find_first(fam, g_prefs, queues[0].f, queues[1].f, queues[2].f);
	if (queues[3].f == NEG_1)
		queues[3] = find_first(fam, g_prefs, queues[0].f, queues[1].f, NEG_1);
	if (queues[3].f == NEG_1)
		queues[3] = find_first(fam, g_prefs, NEG_1, NEG_1, NEG_1);
	if (queues[3].f == NEG_1)
		queues[3] = queues[2];
}

static uint32_t find(std::vector<Queue*> queues, qf_config conf ) {
	for (size_t i = 0; i < queues.size(); ++i)
		if (queues[i]->m_family == conf.f && queues[i]->m_index == conf.i)
			return (uint32_t)i;
	return NEG_1;
}


QueueManager::QueueManager(VkPhysicalDevice physical, VkSurfaceKHR surface) {
	uint32_t size;
	std::vector<VkQueueFamilyProperties> props;
	std::vector<VkBool32> presents;
	std::vector<qf_capabilities> families;
	qf_config queues[4];
	bool supported;

	m_count.clear();
	m_queues.clear();
	for (int i = 0; i < 4; ++i)
		m_qindex[i] = NEG_1;

	vkGetPhysicalDeviceQueueFamilyProperties(physical, &size, nullptr);
	if (!size)
		throw std::runtime_error("No queue families exist!");
	props.resize(size);
	presents.resize(size);
	m_count.resize(size);
	families.resize(size);
	for (uint32_t i = 0; i < size; ++i)
		m_count[i] = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical, &size, props.data());

	for (uint32_t i = 0; i < size; ++i)
		vkGetPhysicalDeviceSurfaceSupportKHR(physical, i, surface, &presents[i]);

	/* Ensure the physical device is actually capable of doing what we need */
	supported = false;
	for (uint32_t i = 0; i < size; ++i)
		if (presents[i] && (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
			supported = true;
			break;
		}
	if (!supported)
		throw std::runtime_error("No queue family exists that support both present and graphics!");
	for (uint32_t i = 0; i < size; ++i)
		if (props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			supported = true;
			break;
		}
	if (!supported)
		throw std::runtime_error("No queue family exists that support general computing!");

	for (uint32_t i = 0; i < size; ++i) {
		families[i].p = presents[i];
		families[i].g = (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) ? 1 : 0;
		families[i].c = (props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) ? 1 : 0;
		families[i].q = props[i].queueCount;
		families[i].o = 0;
	}

	solve(families, queues);
	
	for (uint32_t i = 0; i < 4; ++i) {
		m_qindex[i] = find(m_queues, queues[i]);
		if (m_qindex[i] == NEG_1) {
			m_queues.push_back(new Queue(queues[i].f, queues[i].i));
			m_qindex[i] = (uint32_t)(m_queues.size() - 1);
		}
		m_queues[m_qindex[i]]->addThread((!i) ? 1 : 2);
	}

	for (Queue *q : m_queues)
		m_count[q->m_family]++;

	return;
fail:
	props.clear();
	families.clear();
	presents.clear();
}

QueueManager::~QueueManager() {
	for (Queue *q : m_queues)
		delete q;
	m_queues.clear();
}