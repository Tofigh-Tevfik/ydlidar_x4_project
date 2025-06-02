#ifndef YDLIDAR_X4_READER_HPP
#define YDLIDAR_X4_READER_HPP

#include <vector>
#include <cstdint>
#include <string>

class YDLidarX4Reader {
    private:
    int fd; // file descriptor
    static constexpr const char* SERIAL_PORT = "/dev/ttyUSB0";
    static constexpr int BAUD_RATE = 128000;
    static constexpr size_t BUFFER_SIZE = 64;

    public:
    YDLidarX4Reader();
    ~YDLidarX4Reader();
    void readData();
    int getFileDescriptor() const;
};

#endif