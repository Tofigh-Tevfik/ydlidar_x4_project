#include "ydlidar_x4_reader.hpp"
#include <iostream>

int main() {
    try {
        YDLidarX4Reader lidar;
        lidar.readData();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}