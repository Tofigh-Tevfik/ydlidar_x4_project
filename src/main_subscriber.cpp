#include "zmq/subscriber.hpp"
#include <iostream>

int main() {
    try {
        Subscriber sub;
        std::cout << "Subscribing to Lidar data on tcp:://localhost:5555..." << std::endl;
        while(true) {
            LaserScan scan;
            if (sub.callback(scan)) {
                std::cout << "Received scan at timestamp: " << scan.timestamp << " us" << std::endl;
                for (const auto& point : scan.points) {
                    printf("Angle: %.2f deg, Distance: %.2f mm\n", point.angle, point.distance);
                }
                usleep(1000); // 1ms delay
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}