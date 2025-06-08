#ifndef PUBLISHER_HPP
#define PUBLISHER_HPP

#include "types.hpp"
#include <zmq.hpp>
#include <string>

class Publisher {
    private:
        zmq::context_t context;
        zmq::socket_t socket;

    public:
        Publisher(const std::string& endpoint = "tcp://*:5555");
        void publish(const LidarScan& scan);
};

#endif