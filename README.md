## B1 Depth

Gets depth images from the Unitree B1's cameras and sends them to your computer using zmq.

## Setup

Install cppzmq (and libzmq) as in [the readme of the cppzmq repo](https://github.com/zeromq/cppzmq).
Due to the B1 having older firmware, the nvidia computers may have to build cppzmq without tests.

Install librealsense as well.

On the client, please use the repo [b1-depth client](https://github.com/harelab-ucsc/b1-depth-client) in a ros catkin workspace to read the points and publish them.

To use this repo,
Clone this repo.
On the b1 (server), do:
```cd b1-depth
mkdir build
cd build
cmake ..
make
```
Then run  `./server` .

