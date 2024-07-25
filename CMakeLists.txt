cmake_minimum_required(VERSION 3.14)

project(SimpleWaylandClient 
        VERSION 1.0.0
        LANGUAGES C)

file(GLOB_RECURSE WAYLAND_CLIENT_APPLICATION_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)

add_executable(
    WayClient
    ${WAYLAND_CLIENT_APPLICATION_SOURCES}
)
