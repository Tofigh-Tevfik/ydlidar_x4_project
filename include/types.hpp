#ifndef TYPES_HPP
#define TYPES_HPP

#include <vector>
#include <cstdint>

// a struct for a single data point of lidar
struct LidarPoint {
    float angle; // angle in degrees
    float distance; // distance in millimeters
};

struct LaserScan {
    std::vector<LidarPoint> points; // a vector of lidar points
    uint64_t timestamp; // timestamp in milliseconds
};

#endif