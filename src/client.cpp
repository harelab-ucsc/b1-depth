#include <iostream>
#include <sstream>
#include <stdlib.h>

#include <opencv2/opencv.hpp>
#include <zmq.hpp>
#include <iomanip> // For setting width and padding of the output historgram
#include <algorithm> // For std::max_element
#include <chrono>

#include "msg_helpers.h"

#define DISPLAY_IMAGE true

DepthFrameData deserialize(const uint8_t* data, size_t length) {
    // Ensure the byte array is the correct size
    if (length != sizeof(DepthFrameData)) {
        throw std::runtime_error("Byte array size does not match struct size.");
    }

    DepthFrameData result;
    
    // Copy data from the byte array to the struct
    std::memcpy(&result, data, sizeof(DepthFrameData));
    
    return result;
}

void displayFalseColorArray(const uint16_t* array, int width, int height, const std::string& windowName) {
    // Convert uint16_t array to cv::Mat
    cv::Mat grayMat(height, width, CV_16UC1, const_cast<uint16_t*>(array));

    // Normalize the grayMat to range [0, 255] for color mapping
    cv::Mat normalizedMat;
    double minVal, maxVal;
    cv::minMaxLoc(grayMat, &minVal, &maxVal);
    grayMat.convertTo(normalizedMat, CV_8UC1, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));

    // Apply a colormap
    cv::Mat colorMat;
    cv::applyColorMap(normalizedMat, colorMat, cv::COLORMAP_VIRIDIS); // Use the JET colormap

    // Display the color-mapped image
    cv::imshow(windowName, colorMat);
}

int main (int argc, char *argv[])
{
    zmq::context_t context (1);

    //  Socket to talk to server
    std::cout << "Start listening on localhost...\n" << std::endl;
    zmq::socket_t subscriber (context, zmq::socket_type::sub);
    subscriber.connect("tcp://192.168.123.23:5556");

    subscriber.set(zmq::sockopt::subscribe, "");

    // setup image window
    if (DISPLAY_IMAGE) cv::namedWindow("False Color Depth Image", cv::WINDOW_AUTOSIZE);

    const int num_frames = 1000;
    std::vector<uint64_t> ts_data(num_frames);

    for (int frame_idx=0; frame_idx < num_frames; frame_idx++) {
        zmq::message_t messageData;
        subscriber.recv(messageData, zmq::recv_flags::none);

        if (messageData.size() != sizeof(DepthFrameData)) {
            printf("recieved header before image, flushing next message\n");
            subscriber.recv(messageData, zmq::recv_flags::none);
            break;
        }

        DepthFrameData o = deserialize(messageData.data<uint8_t>(), messageData.size());

        zmq::message_t messageImage;
        subscriber.recv(messageImage, zmq::recv_flags::none);

        printf("ts: %u, our_ts: %u, data size: %u, units: %f, w: %u, h: %u, bpp: %u, \n", o.timestamp, timeMillisec(), o.data_size, o.unitsTimesThousand / 1000.0, o.width, o.height, o.bytes_per_pixel);

        // printf("pixels calc: %u , pixels reciev: %u\n\n", o.width * o.height, o.data_size / o.bytes_per_pixel);

        // printf("their: %u, our_ms: %f\n", o.timestamp, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - 1725697800000.0  );

        if (DISPLAY_IMAGE) {
            displayFalseColorArray(messageImage.data<uint16_t>(), o.width, o.height, "False Color Depth Image");

            // Wait 30 ms for a key press
            char key = (char)cv::waitKey(30); 
            if (key == 27 || key == 'q') { // 27 is the ASCII code for Esc key
                break;
            }
        }
    }

    
    // Close the window
    if (DISPLAY_IMAGE) cv::destroyAllWindows();


    // Number of buckets
    const int numBuckets = 5;

    // Find the maximum value in the data
    int maxValue = *std::max_element(ts_data.begin(), ts_data.end());

    // Calculate bucket size
    int bucketSize = (maxValue / numBuckets) + 1;
    std::vector<int> buckets(numBuckets, 0);

    // Populate buckets
    for (int value : ts_data) {
        if (value >= 0 && value < numBuckets * bucketSize) {
            int bucketIndex = value / bucketSize;
            buckets[bucketIndex]++;
        }
    }

    // Display histogram
    std::cout << "Histogram:\n";
    for (int i = 0; i < numBuckets; ++i) {
        int rangeStart = i * bucketSize;
        int rangeEnd = (i + 1) * bucketSize - 1;
        std::cout << "[" 
                  << std::setw(8) << std::setfill(' ') << rangeStart << "-"
                  << std::setw(8) << std::setfill(' ') << rangeEnd << "] : ";
        std::cout << std::string(buckets[i] / 10, '*') << " (" << buckets[i] << ")\n";
    }

    return 0;
}