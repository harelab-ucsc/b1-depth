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

add_executable(client src/client.cpp)

# OpenCV
find_package(OpenCV)

if (OpenCV_FOUND)
    message(STATUS "OpenCV found.")
    # Include directories and link libraries
    include_directories(${OpenCV_INCLUDE_DIRS})
    link_directories(${OpenCV_LIBRARY_DIRS})

    target_link_libraries(client ${OpenCV_LIBRARIES})

    target_compile_definitions(client PRIVATE HAS_OPENCV=1)
else()
    message(STATUS "OpenCV not found.")
endif()


### REALSENSE LIBRARY

# Find the realsense2 package
pkg_check_modules(REALSENSE2 realsense2)

if (REALSENSE2_FOUND)
    message(STATUS "REALSENSE2 found.")
    # Include directories and link libraries
    include_directories(${REALSENSE2_INCLUDE_DIRS})
    link_directories(${REALSENSE2_LIBRARY_DIRS})

    # use real server
    add_executable(server src/server.cpp)

    target_link_libraries(server ${REALSENSE2_LIBRARIES})
    target_compile_definitions(server PRIVATE HAS_REALSENSE=1)
else()
    message(STATUS "realsense2 not found, using server_dummy.cpp")
    add_executable(server src/server_dummy.cpp)
endif()


#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq REQUIRED)

target_link_libraries(client cppzmq ${OpenCV_LIBS})
target_link_libraries(server cppzmq)
