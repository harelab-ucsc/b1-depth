
#include <librealsense2/rs.hpp> // RealSense Cross Platform API

#include <zmq.hpp>

#include <stdio.h>
#include <iostream>
#include "msg_helpers.h"

int main(int argc, char * argv[]) try
{

    rs2::context ctx;
    // create realsense pipelines (multiple cameras = multiple pipelines)
    std::vector<rs2::pipeline> pipelines;

    // Start a streaming pipe per each connected device
    // https://dev.intelrealsense.com/docs/rs-multicam
    // downward facing camera serial number: 151222079584
    //  forward facing camera serial number: 150622073349
    for (auto&& dev : ctx.query_devices())
    {
        rs2::pipeline pipe(ctx);
        rs2::config cfg;
        cfg.enable_device(dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
        cfg.enable_stream(RS2_STREAM_DEPTH);
        pipe.start(cfg);
        pipelines.emplace_back(pipe);
        std::cout << "Enabling device: " << dev.get_info(RS2_CAMERA_INFO_NAME) << " serial no: " << dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER) << std::endl;
    }


    //  Prepare zmq context and publisher
    zmq::context_t context (1);
    zmq::socket_t publisher (context, zmq::socket_type::pub);
    publisher.bind("tcp://*:5556");

    while (true) {
        // process the new frames from all the connected devices
        for (uint8_t i = 0; i < pipelines.size(); i++)
        {
            rs2::frameset fs;
            if (pipelines[i].poll_for_frames(&fs))
            {
                rs2::depth_frame depth = fs.get_depth_frame();

                // create pointcloud
                rs2::pointcloud pc;
                rs2::points points;

                // get pointcloud
                points = pc.calculate(depth);

                PointCloudInfo info;
                info.timestamp = depth.get_timestamp();
                info.index = i;

                zmq::message_t messageInfo(serialize(info));
                zmq::message_t messagePoints(points.get_data(), points.get_data_size());

                publisher.send(messageInfo, zmq::send_flags::sndmore);
                publisher.send(messagePoints, zmq::send_flags::none);
            }
        }
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