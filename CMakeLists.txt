cmake_minimum_required(VERSION 3.14)

project(SimpleWaylandClient 
        VERSION 1.0.0
        LANGUAGES C)

find_program(HAS_wayland-scanner wayland-scanner)
if (NOT HAS_wayland-scanner)
    message(FATAL_ERROR "wayland-scanner not found!")
endif()
execute_process(
    COMMAND wayland-scanner
            client-header
            /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
            ${PROJECT_SOURCE_DIR}/src/internals/protocols/xdg-shell-client-protocol.h
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error_output
)
execute_process(
    COMMAND wayland-scanner
            client-header
            /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml
            ${PROJECT_SOURCE_DIR}/src/internals/protocols/xdg-decoration-protocol.h
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error_output
)
execute_process(
    COMMAND wayland-scanner
            client-header
            /usr/share/wayland-protocols/unstable/relative-pointer/relative-pointer-unstable-v1.xml
            ${PROJECT_SOURCE_DIR}/src/internals/protocols/relative-pointer-protocol.h
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error_output
)
execute_process(
    COMMAND wayland-scanner
            client-header
            /usr/share/wayland-protocols/stable/viewporter/viewporter.xml
            ${PROJECT_SOURCE_DIR}/src/internals/protocols/viewporter-protocol.h
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error_output
)

execute_process(
    COMMAND wayland-scanner
            public-code
            /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
            ${PROJECT_SOURCE_DIR}/src/internals/protocols/xdg-shell-protocol.c
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error_output
)
execute_process(
    COMMAND wayland-scanner
            public-code
            /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml
            ${PROJECT_SOURCE_DIR}/src/internals/protocols/xdg-decoration-protocol.c
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error_output
)
execute_process(
    COMMAND wayland-scanner
            public-code
            /usr/share/wayland-protocols/unstable/relative-pointer/relative-pointer-unstable-v1.xml
            ${PROJECT_SOURCE_DIR}/src/internals/protocols/relative-pointer-protocol.c
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error_output
)
execute_process(
    COMMAND wayland-scanner
            public-code
            /usr/share/wayland-protocols/stable/viewporter/viewporter.xml
            ${PROJECT_SOURCE_DIR}/src/internals/protocols/viewporter-protocol.c
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error_output
)


file(GLOB_RECURSE WAYLAND_CLIENT_APPLICATION_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)

add_executable(
    WayClient
    ${WAYLAND_CLIENT_APPLICATION_SOURCES}
)

target_link_libraries(WayClient PRIVATE wayland-client wayland-egl EGL rt xkbcommon m)
