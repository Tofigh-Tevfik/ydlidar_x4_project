#include "publisher.hpp"
#include <stdexcept>
#include <iostream>

Publisher::Publisher(const std::string& endpoint) : context(1), socket(context, ZMQ_PUB) {
    try {
        socket.bind(endpoint);
    } catch (const zmq::error_t& e) {
        throw std::runtime_error("Failed to bind publisher socket: " + std::string(e.what()));
    }
}

void Publisher::publish(const LaserScan& scan) {
    // send topic
    zmq::message_t topic("lidar_scan", 10);
    socket.send(topic, zmq::send_flags::sndmore);

    // validate points size
    size_t points_size = scan.points.size();
    if (points_size > 1000) { 
        throw std::runtime_error("Invalid points size: " + std::to_string(points_size));
    }

    // for debug: print scan info
    std::cout << "Publishing scan with " << points_size << " points, timestamp: " << scan.timestamp << std::endl;

    // serialize LidarScan: timestamp + number of points + points
    size_t buffer_size = sizeof(uint64_t) + sizeof(size_t) + points_size * sizeof(LidarPoint);
    std::vector<uint8_t> buffer(buffer_size);

    size_t offset = 0;
    std::memcpy(buffer.data() + offset, &scan.timestamp, sizeof(uint64_t));
    offset += sizeof(uint64_t);
    std::memcpy(buffer.data() + offset, &points_size, sizeof(size_t));
    offset += sizeof(size_t);
    if (points_size > 0) {
        std::memcpy(buffer.data() + offset, scan.points.data(), points_size * sizeof(LidarPoint));
    }


    // send data
    zmq::message_t message(buffer.data(), buffer_size);
    socket.send(message, zmq::send_flags::none);
}