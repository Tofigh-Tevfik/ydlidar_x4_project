find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBZMQ REQUIRED libzmq)

add_library(zmq_nodes
    publisher.cpp
    subscriber.cpp
)

target_include_directories(zmq_nodes PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${LIBZMQ_INCLUDE_DIRS}
)
target_link_libraries(zmq_nodes PUBLIC
    ${LIBZMQ_LIBRARIES}
)