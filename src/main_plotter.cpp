#include "plotter.hpp"
#include <iostream>

int main() {
    try {
        Plotter plotter;
        std::cout << "Plotting Lidar data from tcp:://localhost:5555..." << std::endl;
        plotter.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}