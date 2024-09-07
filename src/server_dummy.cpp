#include <zmq.hpp>
#include <stdlib.h>

#include "msg_helpers.h"

int main () {

    //  Prepare our context and publisher
    zmq::context_t context;
    zmq::socket_t publisher (context, zmq::socket_type::pub);
    printf("Server dummy publishing dummy images...\n");
    publisher.bind("tcp://*:5556");

    int offset = 0; // for sample data

    while (1) {
        DepthFrameData data;
        data.timestamp = timeMillisec();
        data.bytes_per_pixel = 2;
        data.width = 600;
        data.height = 400;
        data.data_size = data.bytes_per_pixel * data.width * data.height;
        data.unitsTimesThousand = 10;

        offset++;

        uint16_t *ptr = (uint16_t*)malloc(data.data_size);
        if (ptr == NULL) {
            printf("unable to allocate pointer \n");
            continue;
        }

        // Fill the array with some sample data (e.g., a gradient)
        for (int y = 0; y < data.height; ++y) {
            for (int x = 0; x < data.width; ++x) {
                ptr[y*data.width+x] = static_cast<uint16_t>((x + y + offset) % 256);
            }
        }

        zmq::message_t messageD(serialize(data));
        publisher.send(messageD, zmq::send_flags::none);


        zmq::message_t messageI(ptr, data.data_size);
        publisher.send(messageI, zmq::send_flags::none);
    }
    return 0;
}