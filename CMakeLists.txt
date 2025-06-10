cmake_minimum_required(VERSION 3.10)
project(ydlidar_x4_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src/ydlidar)
add_subdirectory(src/zmq)
add_subdirectory(src/plot)

add_executable(ydlidar_x4_publisher src/main_publisher.cpp)
target_link_libraries(ydlidar_x4_publisher ydlidar zmq_nodes)

add_executable(ydlidar_x4_subscriber src/main_subscriber.cpp)
target_link_libraries(ydlidar_x4_subscriber zmq_nodes)

add_executable(ydlidar_x4_plotter src/main_plotter.cpp)
target_link_libraries(ydlidar_x4_plotter plot)