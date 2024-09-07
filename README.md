## B1 Depth

Gets depth images from the Unitree B1's cameras and sends them to your computer using zmq.

## Setup

Install cppzmq (and libzmq) as in [the readme of the cppzmq repo](https://github.com/zeromq/cppzmq).
Due to the B1 having older firmware, the nvidia computers may have to build cppzmq without tests.

Install librealsense, or if you just want to run a test without realsense, do not install/set HAS_REALSENSE to false. In that case dummy data will be sent.

On the client, install openCV if you would like there to be display, or do not install on the headless computers.

Clone this repo.
On the client and server, do:
`cd b1-depth`
`mkdir build`
`cd build`
`cmake ..`
`make`
Then do `./client` or `./server` respectively.

