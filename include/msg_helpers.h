
#ifndef MSG_HELPERS_HARE
#define MSG_HELPERS_HARE

#include <chrono>

inline uint32_t timeMillisec() {
    using namespace std::chrono;
    // cast to uint32 from uint64: top 4 bytes are truncated...
    return static_cast<uint32_t>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - 1725697800000);
}

struct __attribute__((packed)) DepthFrameData {
    uint32_t timestamp;
    uint32_t data_size;
    uint16_t unitsTimesThousand;
    uint16_t width;
    uint16_t height;
    uint8_t bytes_per_pixel;
};

#endif // MSG_HELPERS_HARE
