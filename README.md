# YDLidar X4 Project

## Overview
The **YDLidar X4 Project** is a C++ application designed to interface with the YDLidar X4 LiDAR sensor, enabling real-time data acquisition, communication, and visualization.

The project reads raw serial data from the LiDAR, parses it into LaserScan messages (containing angles and distances), and publishes these messages using ZeroMQ’s publisher/subscriber pattern. A subscriber node receives the data, and an SFML-based visualization node plots the LiDAR scans as a 2D Cartesian point cloud.

This project demonstrates robust serial communication, distributed messaging, and graphical rendering, making it a valuable learning tool for robotics and sensor interfacing.

### Key Features
- **Serial Data Parsing**: Reads and validates YDLidar X4 packets (header `0xAA 0x55`, CT, LSN, FSA/LSA, checksum, samples) to extract angles (degrees) and distances (millimeters).
- **ZeroMQ Messaging**: Publishes `LaserScan` messages over `tcp://*:5555` and subscribes to them, enabling decoupled data exchange.
- **SFML Visualization**: Renders LiDAR points in a real-time 2D Cartesian view using SFML, displaying green dots scaled to fit an 800x600 window.
- **Modular Design**: Organized into `ydlidar`, `zmq`, and `plot` modules for maintainability.

## Tech Stack
- **C++17** for core logic  
- **ZeroMQ** (`libzmq`) for publisher/subscriber communication  
- **SFML** (`libsfml`) for graphical visualization  
- **CMake** for build management

## Prerequisites

### Hardware
- YDLidar X4 LiDAR sensor connected to `/dev/ttyUSB0`
- USB-to-serial adapter  
  *(Ensure permissions with: `sudo usermod -a -G dialout $USER` and then **log out/in**)*
  
### Software
- Ubuntu 20.04 or later (tested)
- C++ compiler (e.g., `g++` with C++17 support)
- CMake 3.10 or higher
- Required Libraries:
  ```bash
  sudo apt-get install build-essential cmake libzmq3-dev libsfml-dev
  ```

### Permissions
Ensure the user has access to `/dev/ttyUSB0`:
```bash
sudo usermod -a -G dialout $USER
# Log out and log back in for the group change to take effect
```

## Build Instructions

### Clone the Repository
```bash
git clone https://github.com/Tofigh-Tevfik/ydlidar_x4_project.git
cd ydlidar_x4_project
```

### Create Build Directory
```bash
mkdir build && cd build
```

### Configure and Build
```bash
cmake ..
make
```

## Run the Nodes

### Publisher (Publishes LaserScan data)
```bash
sudo ./ydlidar_x4_publisher
```
- Requires `sudo` for access to `/dev/ttyUSB0`
- Outputs parsed angles and distances to the terminal

### Subscriber (Receives and prints LaserScan data)
```bash
./ydlidar_x4_subscriber
```
- Run in a **separate terminal**

### Plotter (Visualizes LaserScan as a 2D point cloud)
```bash
./ydlidar_x4_plotter
```
- Displays a window with green points representing LiDAR scans
- Close the window to exit

### Plotting LiDAR Data from a Remote Device

To visualize LiDAR data from another device on the same local network, update the plotter's endpoint from:

```
"tcp://localhost:5555"
```

to the IP address of the remote device, for example:

```
"tcp://192.168.1.21:5555"
```

Replace `192.168.1.21` with the actual IP address of the device running the LiDAR publisher.

## Demo Video
[![Watch the Demo](https://img.shields.io/badge/YouTube-Watch%20Demo-red?logo=youtube)](https://youtu.be/TBC5uqdGn5M)


## Contact

For questions, issues, or contributions:

- **EMAIL**: <a href="mailto:tofigh.main@gmail.com">Mail me</a>

## Upcoming Updates

The project is complete in its current form, but the following documentation efforts are planned:
- **Project Summary Document**: A detailed report summarizing the project’s architecture, implementation, and lessons learned. (Done)
- **Demonstrative Video**: A short video showcasing the LiDAR data acquisition, ZeroMQ messaging, and SFML visualization in action.

> No additional code updates are planned at this time.
