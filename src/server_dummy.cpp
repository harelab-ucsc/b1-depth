#include <zmq.hpp>
#include <stdlib.h>

#include "msg_helpers.h"

int main () {

    //  Prepare our context and publisher
    zmq::context_t context;
    zmq::socket_t publisher (context, zmq::socket_type::pub);
    publisher.bind("tcp://*:5556");

    int offset = 0; // for sample data

    while (1) {
        DepthFrameData data;
        data.timestamp = timeMillisec();
        data.bytes_per_pixel = 2;
        data.width = 100;
        data.height = 100;
        data.data_size = data.bytes_per_pixel * data.width * data.height;
        // TODO: test this server example

        uint16_t *ptr = (uint16_t*)malloc(data.data_size);

        // Fill the array with some sample data (e.g., a gradient)
        for (int y = 0; y < data.height; ++y) {
            for (int x = 0; x < data.width; ++x) {
                ptr[x+y] = static_cast<uint16_t>((x + y + offset) % 256);
            }
        }
        offset++;

        zmq::message_t messageD(serialize(data));
        publisher.send(messageD, zmq::send_flags::none);


        zmq::message_t messageI(ptr, data.data_size);
        publisher.send(messageI, zmq::send_flags::none);
    }
    return 0;
}