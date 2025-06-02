cmake_minimum_required(VERSION 3.10)
project(YDLidarX4Project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include directories
include_directories(${CMAKE_SOURCE_DIR}/src/ydlidar)

# lidar module
add_subdirectory(src/ydlidar)

# main exec
add_executable(ydlidar_x4_app src/main.cpp)
target_link_libraries(ydlidar_x4_app ydlidar)