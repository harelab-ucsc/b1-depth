
#ifndef MSG_HELPERS_HARE
#define MSG_HELPERS_HARE

#include <chrono>

inline uint64_t timeSinceEpochMillisec() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

struct DepthFrame {
    uint64_t timestamp;
    int height = 100;
    int width = 400;
    float depths[100][400];
};

#endif // MSG_HELPERS_HARE
