#include "plotter.hpp"
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Plotter::Plotter(const std::string& endpoint) 
    : sub(endpoint), window(sf::VideoMode(800, 600), "YDLidar X4 Scan"),
      scale(1.f), origin(400.f, 300.f) {
    window.setFramerateLimit(60);
}

void Plotter::run() {
    while (window.isOpen()) {
        // event handler
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // receive scan
        LaserScan scan;
        if (sub.callback(scan)) {
            std::cout << "Plotting scan with " << scan.points.size() << " points, timestamp: "
                      << scan.timestamp << std::endl;

            // clear window
            window.clear(sf::Color::Black);

            // draw points
            for (const auto& point : scan.points) {
                float angle = point.angle;
                // some angles exceed the value 360.0
                if (angle > 360.f) {
                    angle -= 360.f;
                }

                float angle_rad = angle * M_PI / 180.f;           
                float x = point.distance * std::cos(angle_rad) * scale;
                float y = point.distance * std::sin(angle_rad) * scale;
                sf::CircleShape dot(2.f);
                dot.setFillColor(sf::Color::Green);
                dot.setPosition(origin.x + x - 2.f, origin.y - y - 2.f);
                window.draw(dot);
            }
        }
        window.display();
    }
}