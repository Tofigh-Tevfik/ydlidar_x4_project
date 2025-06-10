#ifndef PLOTTER_HPP
#define PLOTTER_HPP

#include "types.hpp"
#include "../zmq/subscriber.hpp"
#include <SFML/Graphics.hpp>

class Plotter {
    private:
        Subscriber sub;
        sf::RenderWindow window;
        float scale; // pixels per mm
        sf::Vector2f origin;

    public:
        Plotter(const std::string& endpoint = "tcp://localhost:5555");
        void run();
};

#endif