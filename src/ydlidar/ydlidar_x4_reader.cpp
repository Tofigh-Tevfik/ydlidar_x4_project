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
    LaserScan scan;
    scan.timestamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count()
    );

    // Step 1: Wait for header (0xAA 0x55)
    uint8_t header[2];
    while (true) {
        if (read(fd, header, 2) != 2) continue;
        if (header[0] == 0xAA && header[1] == 0x55) break;
    }

    // Step 2: Read header fields (CT, LSN, FSA, LSA, CS)
    uint8_t meta[8];
    if (read(fd, meta, 8) != 8) {
        throw std::runtime_error("Failed to read packet meta");
    }

    uint8_t ct = meta[0];
    uint8_t lsn = meta[1];
    uint16_t fsa = (meta[3] << 8) | meta[2];
    uint16_t lsa = (meta[5] << 8) | meta[4];
    uint16_t cs_read = (meta[7] << 8) | meta[6];

    if (lsn == 0) throw std::runtime_error("Invalid LSN: 0");

    // Step 3: Read sample data
    std::vector<uint8_t> samples(lsn * 2);
    if (read(fd, samples.data(), samples.size()) != static_cast<ssize_t>(samples.size())) {
        throw std::runtime_error("Failed to read sample data");
    }

    // Step 4: Checksum calculation (XOR of CT to last Si byte)
    uint16_t cs_calc = 0;
    for (int i = 0; i < 6; ++i) cs_calc ^= meta[i];  // CT to LSA
    for (auto b : samples) cs_calc ^= b;

    if (cs_calc != cs_read) {
        throw std::runtime_error("Checksum mismatch");
    }

    // Step 5: Decode angles and distances
    float angle_fsa = (fsa >> 1) / 64.0f;
    float angle_lsa = (lsa >> 1) / 64.0f;
    float angle_diff = angle_lsa - angle_fsa;
    if (angle_diff < 0) angle_diff += 360.0f;
    float angle_step = angle_diff / (lsn - 1);

    scan.points.reserve(lsn);
    for (size_t i = 0; i < lsn; ++i) {
        uint16_t raw = (samples[2 * i + 1] << 8) | samples[2 * i];
        float dist_mm = raw / 4.0f;
        float angle = angle_fsa + i * angle_step;
        scan.points.push_back({angle, dist_mm});
    }

    return scan;
}

int YDLidarX4Reader::getFileDescriptor() const {
    return fd;
}