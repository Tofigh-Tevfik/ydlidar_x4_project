find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_library(plot
    plotter.cpp
)

target_include_directories(plot PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(plot PUBLIC
    zmq_nodes
    sfml-graphics
    sfml-window
    sfml-system
)