#include <zmq.hpp>
#include <iostream>
#include <sstream>
#include <stdlib.h>

struct DepthFrame {
    int32_t timestamp;
    float depths[2][2];
};

DepthFrame deserialize(const uint8_t* data, size_t length) {
    // Ensure the byte array is the correct size
    if (length != sizeof(DepthFrame)) {
        throw std::runtime_error("Byte array size does not match struct size.");
    }

    DepthFrame result;
    
    // Copy data from the byte array to the struct
    std::memcpy(&result, data, sizeof(DepthFrame));
    
    return result;
}

int main (int argc, char *argv[])
{
    zmq::context_t context (1);

    //  Socket to talk to server
    std::cout << "Start listening on localhost...\n" << std::endl;
    zmq::socket_t subscriber (context, zmq::socket_type::sub);
    subscriber.connect("tcp://localhost:5556");

    subscriber.set(zmq::sockopt::subscribe, "");

    for (int update_nbr = 0; update_nbr < 100; update_nbr++) {

        zmq::message_t message;
        subscriber.recv(message, zmq::recv_flags::none);

        DepthFrame o = deserialize(message.data<uint8_t>(), message.size());

        printf("Depth frame, ts: %d, 1: %f, 2: %f, 3: %f, 4: %f\n", o.timestamp, o.depths[0][0], o.depths[0][1], o.depths[1][0], o.depths[1][1]);
    }

    return 0;
}