cmake_minimum_required(VERSION 3.10)

project(
  b1_depth
  VERSION 1.0
  LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(client src/client.cpp)
add_executable(server src/server.cpp)

# OpenCV
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq)


target_link_libraries(client cppzmq ${OpenCV_LIBS})
target_link_libraries(server cppzmq)
