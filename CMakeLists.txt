cmake_minimum_required(VERSION 3.10)

project(
  b1_depth
  VERSION 1.0
  LANGUAGES CXX)

add_executable(main main.cpp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq)
target_link_libraries(main cppzmq)
