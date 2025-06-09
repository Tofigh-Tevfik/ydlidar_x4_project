find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBZMQ REQUIRED libzmq)

add_library(zmg
    publisher.cpp
    subscriber.cpp
)

target_include_directories(zmq PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${LIBZMQ_INCLUDE_DIRS}
)

target_link_libraries(zmq PUBLIC
    ${LIBZMQ_LIBRARIES}
)
