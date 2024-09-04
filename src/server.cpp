#include <zmq.hpp>
#include <stdlib.h>

#include "msg_helpers.h"

std::vector<uint8_t> serialize(const DepthFrame& data) {
    // Create a byte vector with enough space
    std::vector<uint8_t> byteArray(sizeof(DepthFrame));
    
    // Copy data from the struct to the byte array
    std::memcpy(byteArray.data(), &data, sizeof(DepthFrame));

    return byteArray;
}

int main () {

    //  Prepare our context and publisher
    zmq::context_t context (1);
    zmq::socket_t publisher (context, zmq::socket_type::pub);
    publisher.bind("tcp://*:5556");

    int offset = 0;

    while (1) {
        DepthFrame data;

        data.timestamp = timeSinceEpochMillisec();

        printf("%lu\n", timeSinceEpochMillisec());

        // Fill the array with some sample data (e.g., a gradient)
        for (int y = 0; y < data.height; ++y) {
            for (int x = 0; x < data.width; ++x) {
                data.depths[y][x] = static_cast<float>((x + y + offset) % 100);
            }
        }

        offset++;

        zmq::message_t message(serialize(data));
        publisher.send(message, zmq::send_flags::none);
    }
    return 0;
}