#include "ydlidar_x4_reader.hpp"
#include "serial_termios2_linux.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cerrno>

YDLidarX4Reader::YDLidarX4Reader() {
    // opening serial port
    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        throw std::runtime_error("Failed to open serial port: " + std::string(strerror(errno)));
    }

    // configure serial port
    struct termios2 options;
    if (ioctl(fd, TCGETS2, &options) != 0) {
        close(fd);
        throw std::runtime_error("Failed to get serial port attributes: " + std::string(strerror(errno)));
    }

    // setting baud rate to 128000
    options.c_cflag &= ~CBAUD; // clear standard baud rate
    options.c_cflag |= BOTHER; // allow custom baud rate
    options.c_ispeed = 128000; // Input speed
    options.c_ospeed = 128000; // Output speed
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag |= (CLOCAL | CREAD); // enable receiver, local mode
    options.c_cflag &= ~(PARENB | PARODD);
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    options.c_cflag &= ~(IXON | IXOFF | IXANY); // no flow control
    options.c_oflag = 0;
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 10; // 1 second timeout

    if (ioctl(fd, TCSETSF2, &options) != 0) {
        close(fd);
        throw std::runtime_error("Failed to set serial port attributes: " + std::string(strerror(errno)));
    }

    // flush the port
    tcflush(fd, TCIOFLUSH);

    uint8_t start_cmd[] = {0xA5, 0x60};
    if (write(fd, start_cmd, sizeof(start_cmd)) != sizeof(start_cmd)) {
        close(fd);
        throw std::runtime_error("Failed to send start command: " + std::string(strerror(errno)));
    }
    usleep(100000); // 0.1s delay
}

YDLidarX4Reader::~YDLidarX4Reader() {
    if (fd >= 0) {
        close(fd);
    }
}

void YDLidarX4Reader::readData() {
    std::vector<uint8_t> buffer(BUFFER_SIZE);
    std::cout << "LIDAR is spinning. Reading data..." << std::endl;

    while (true) {
        ssize_t bytes_read = read(fd, buffer.data(), BUFFER_SIZE);
        if (bytes_read > 0) {
            for (ssize_t i = 0; i < bytes_read; ++i) {
                printf("%02x ", buffer[i]);
            }
            printf("\n");
            fflush(stdout);
        } else if (bytes_read < 0) {
            std::cerr << "Error reading from serial port: " + std::string(strerror(errno)) << std::endl;
        }
        usleep(1000);
    }
}

int YDLidarX4Reader::getFileDescriptor() const {
    return fd;
}