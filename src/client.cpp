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

void displayFalseColorArray(const float* array, int width, int height, const std::string& windowName) {
    
    // Convert float data to 8-bit unsigned integers
    cv::Mat floatMat(height, width, CV_32FC1, const_cast<float*>(array)); // Original float data
    cv::Mat grayMat(height, width, CV_8UC1); // Destination for 8-bit grayscale image

    // Find min and max values in the float array
    double minVal, maxVal;
    cv::minMaxLoc(floatMat, &minVal, &maxVal);

    // Normalize the float data to [0, 255] range and convert to 8-bit
    floatMat.convertTo(grayMat, CV_8UC1, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));

    // Apply a colormap
    cv::Mat colorMat;
    cv::applyColorMap(grayMat, colorMat, cv::COLORMAP_JET); // Use the JET colormap

    // Display the color-mapped image
    cv::imshow(windowName, colorMat);
}

int main (int argc, char *argv[])
{
    zmq::context_t context (1);

    //  Socket to talk to server
    std::cout << "Start listening on localhost...\n" << std::endl;
    zmq::socket_t subscriber (context, zmq::socket_type::sub);
    subscriber.connect("tcp://localhost:5556");

    subscriber.set(zmq::sockopt::subscribe, "");

    // setup image window
    if (DISPLAY_IMAGE) cv::namedWindow("False Color Depth Image", cv::WINDOW_AUTOSIZE);

    const int num_frames = 100;
    std::vector<uint64_t> ts_data(num_frames);

    for (int frame_idx=0; frame_idx < num_frames; frame_idx++) {
        zmq::message_t message;
        subscriber.recv(message, zmq::recv_flags::none);

        DepthFrame o = deserialize(message.data<uint8_t>(), message.size());

        ts_data[frame_idx] = timeMillisec() - o.timestamp;

        printf("%u,     %u\n", timeMillisec() - o.timestamp,  o.timestamp);

        if (DISPLAY_IMAGE) {
            displayFalseColorArray(&o.depths[0][0], o.width, o.height, "False Color Depth Image");

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