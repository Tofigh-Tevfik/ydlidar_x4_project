#include "subscriber.hpp"
#include <stdexcept>

Subscriber::Subscriber(const std::string& endpoint) : context(1), socket(context, ZMQ_SUB) {
    try {
        socket.connect(endpoint);
        socket.set(zmq::option::subscribe, "lidar_scan");
    } catch (const zmq::error_t& e) {
        throw std::runtime_error("Failed to connect subscriber socket: " + std::string(e.what()));
    }
}

bool Subscriber::callback(LidarScan& scan) {
    // receive topic
    zmq::message_t topic;
    if (!socket.receive(topic)) return false;

    // receive data
    zmq::message_t msg;
    if (!socket.receive(msg)) return false;

    // deserialize
    const uint8_t* data = static_cast<const uint8_t*>(msg.data());
    size_t offset = 0;
    std::memcpy(&scan.timestamp, data + offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);
    size_t point_size;
    std::memcpy(&points_size, data + offset, sizeof(size_t));
    offset += sizeof(size_t);
    scan.points.resize(points_size);
    std::memcpy(scan.points.data(), data + offset, points_size * sizeof(LidarPoint));

    return true;
}