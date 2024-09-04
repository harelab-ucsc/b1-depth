
#ifndef MSG_HELPERS_HARE
#define MSG_HELPERS_HARE

#include <chrono>

inline uint32_t timeMillisec() {
    using namespace std::chrono;
    // cast to uint32 from uint64: top 4 bytes are truncated...
    return static_cast<uint32_t>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
}

struct DepthFrame {
    uint32_t timestamp;
    uint16_t height = 100;
    uint16_t width = 400;
    float depths[100][400];
};

#endif // MSG_HELPERS_HARE
