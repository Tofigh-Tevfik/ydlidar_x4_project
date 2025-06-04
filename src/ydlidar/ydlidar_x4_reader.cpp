#include "ydlidar_x4_reader.hpp"
#include "serial_termios2_linux.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <chrono>

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
    options.c_ispeed = BAUD_RATE; // Input speed
    options.c_ospeed = BAUD_RATE; // Output speed
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag |= (CLOCAL | CREAD); // enable receiver, local mode
    options.c_cflag &= ~(PARENB | PARODD);
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 10; // 1 second timeout
    options.c_iflag = 0;       // fully clear input modes
    options.c_lflag = 0;       // no local modes (canonical, echo, etc.)
    options.c_oflag = 0;       // no output processing

    if (ioctl(fd, TCSETSF2, &options) != 0) {
        close(fd);
        throw std::runtime_error("Failed to set serial port attributes: " + std::string(strerror(errno)));
    }

    // flush the port
    tcflush(fd, TCIOFLUSH);

    // start scan
    uint8_t start_cmd[] = {0xA5, 0x60};
    if (write(fd, start_cmd, sizeof(start_cmd)) != sizeof(start_cmd)) {
        close(fd);
        throw std::runtime_error("Failed to send start command: " + std::string(strerror(errno)));
    }

    tcflush(fd, TCIOFLUSH);
}

YDLidarX4Reader::~YDLidarX4Reader() {
    if (fd >= 0) {
        close(fd);
    }
}

void YDLidarX4Reader::readData() {
    std::cout << "LIDAR is spinning. Reading data..." << std::endl;
    while (true) {
        LaserScan scan = parseData();
        for (const auto& point : scan.points) {
            printf("Angle: %.2f deg, Distance: %.2f mm\n", point.angle, point.distance);
        }
        usleep(1000);
    }
}


LaserScan YDLidarX4Reader::parseData() {
    std::vector<uint8_t> buffer(BUFFER_SIZE);
    LaserScan scan;
    scan.timestamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count()
    );

    // finding packet header (0x55AA)
    while (true) {
        ssize_t bytes_read = read(fd, buffer.data(), 2);
        if (bytes_read < 2) continue;
        if (buffer[0] == 0x55 && buffer[1] == 0xAA) break;
    }

    // reading packer header (8 bytes: type, count, start/end angles)
    buffer.resize(8);
    if (read(fd, buffer.data(), 8) != 8) {
        throw std::runtime_error("Failed to read packet header");
    }

    uint8_t type = buffer[0];
    uint8_t sample_count = buffer[1];
    uint16_t start_angle = (buffer[3] << 8) | buffer[2];
    uint16_t end_angle = (buffer[5] << 8) | buffer[4];
    uint8_t checksum = buffer[7];

    // validation
    if (type != 0x01) {
        throw std::runtime_error("Invalid packet type: " + std::to_string(type));
    }

    // reading data points
    buffer.resize(sample_count * 2);
    if (read(fd, buffer.data(), sample_count * 2) != sample_count * 2) {
        throw std::runtime_error("Failed to read data points");
    }

    // compute angles and distances
    float start_angle_deg = start_angle / 64.0f;
    float end_angle_deg = end_angle / 64.0f;
    float angle_step = (end_angle_deg - start_angle_deg) / (sample_count - 1);

    scan.points.reserve(sample_count);
    for (size_t i = 0; i < sample_count; ++i) {
        uint16_t distance = (buffer[i * 2 + 1] << 8) | buffer[i * 2];
        float angle = start_angle_deg + i * angle_step;
        scan.points.push_back({angle, static_cast<float>(distance)});
    }

    // validating checksum (XOR of data bytes)
    uint8_t calc_checksum = 0x55 ^ 0xAA;
    for (size_t i = 0; i < 6; ++i) calc_checksum ^= buffer[i];
    for (size_t i = 0; i < sample_count * 2; i++) calc_checksum ^= buffer[i];
    if (calc_checksum != checksum) {
        throw std::runtime_error("Checksum mismatch");
    }

    return scan;
}

int YDLidarX4Reader::getFileDescriptor() const {
    return fd;
}