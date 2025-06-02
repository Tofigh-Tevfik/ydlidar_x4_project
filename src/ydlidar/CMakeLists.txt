add_library(ydlidar
    ydlidar_x4_reader.cpp
)
target_include_directories(ydlidar PUBLIC ${CMAKE_SOURCE_DIR}/include)