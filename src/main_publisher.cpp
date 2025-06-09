#include "ydlidar/ydlidar_x4_reader.hpp"
#include "zmq/publisher.hpp"
#include "iostream"

int main() {
    try {
        YDLidarX4Reader lidar;
        Publisher pub;
        std::cout << "Publishing Lidara data on tcp://*:5555..." << std::endl;
        while(true) {
            LaserScan scan = lidar.parseData();
            pub.publish(scan);
            for (const auto& point : scan.points) {
                printf("Published - Angle: %.2f deg, Distance: %.2f mm\n", point.angle, point.distance);
            }
            usleep(1000); // 1ms delay
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}