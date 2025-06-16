cmake_minimum_required(VERSION 3.10)

project(
  b1_depth
  VERSION 1.0
  LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# specify the pkg-config path
set(PKG_CONFIG_PATH "/usr/local/lib/pkgconfig")
find_package(PkgConfig REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

### REALSENSE LIBRARY

# Find the realsense2 package
pkg_check_modules(REALSENSE2 realsense2)

# Include directories and link libraries
include_directories(${REALSENSE2_INCLUDE_DIRS})
link_directories(${REALSENSE2_LIBRARY_DIRS})

add_executable(server src/server.cpp)

target_link_libraries(server ${REALSENSE2_LIBRARIES})

#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq REQUIRED)

target_link_libraries(server cppzmq)
