
#include <librealsense2/rs.hpp> // RealSense Cross Platform API

#include <zmq.hpp>

#include <stdio.h>
#include <iostream>
#include "msg_helpers.h"

int main(int argc, char * argv[]) try
{

    // create realsense pipeline
    rs2::pipeline p;

    rs2::config config;
    // downward facing camera serial number: 151222079584
    //  forward facing camera serial number: 150622073349
    config.enable_device("150622073349"); 
    config.enable_stream(RS2_STREAM_DEPTH);
    p.start(config);


    //  Prepare zmq context and publisher
    zmq::context_t context (1);
    zmq::socket_t publisher (context, zmq::socket_type::pub);
    publisher.bind("tcp://*:5556");

    while (true) {

        // Block program until frames arrive
        rs2::frameset frames = p.wait_for_frames();

        // Try to get a frame of a depth image
        rs2::depth_frame depth = frames.get_depth_frame();

        uint32_t timestamp = static_cast<uint32_t>(depth.get_timestamp() - 1725697800000.0);
        uint32_t data_size = depth.get_data_size();
        uint16_t unitsTimesThousand = static_cast<uint16_t>(depth.get_units() * 1000);
        uint16_t width = depth.get_width();
        uint16_t height = depth.get_height();
        uint8_t bytes_per_pixel = depth.get_bytes_per_pixel();



        // printf("ts domain: %d, ", depth.get_frame_timestamp_domain());
        // "Frame timestamp was measured in relation to the camera clock and converted to OS system clock by constantly measure the difference "
        
        // union {
        //     double d;
        //     uint64_t u;
        // } converter;
        // converter.d = depth.get_timestamp();
        
        // // Extract the lower 32 bits (mantissa is lower 52 bits ieee754)
        // printf("ts_truc: %u  ", static_cast<uint32_t>(converter.u));

        printf("ts og: %.5f, ts: %u, data size: %u, units: %f, w: %u, h: %u, bpp: %u   \r", depth.get_timestamp(), timestamp, data_size, unitsTimesThousand / 1000.0, width, height, bytes_per_pixel);

        DepthFrameData data;

        data.timestamp = timestamp;
        data.data_size = data_size;
        data.unitsTimesThousand = unitsTimesThousand;
        data.width = width;
        data.height = height;
        data.bytes_per_pixel = bytes_per_pixel;

        zmq::message_t messageData(serialize_depthframe(data));
        zmq::message_t messageImage(depth.get_data(), depth.get_data_size());

        publisher.send(messageData, zmq::send_flags::sndmore);
        publisher.send(messageImage, zmq::send_flags::none);

        // uint32_t ts = timeMillisec();
        // zmq::message_t m(&ts, 4);
        // publisher.send(m, zmq::send_flags::none);
    }

    return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}