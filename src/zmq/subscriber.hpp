#ifndef SUBSCRIBER_HPP
#define SUBSCRIBER_HPP

#include "types.hpp"
#include <zmq.hpp>
#include <string>

class Subscriber {
    private:
        zmg::context_t context;
        zmq::socket_t socket;

    public:
        Subscriber(const std::string& endpoint = "tcp://localhost:5555");
        bool callback(LidarScan& scan);
};

#endif