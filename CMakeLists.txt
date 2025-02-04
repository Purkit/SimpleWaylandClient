cmake_minimum_required(VERSION 3.14)

project(SimpleWaylandClient 
        VERSION 1.0.0
        LANGUAGES C)

# For genetaring wayland protocol header + source files:
#wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg-shell-client-protocol.h
#wayland-scanner public-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg-shell-protocol.h
#wayland-scanner client-header /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml xdg-decoration-protocol.h
#wayland-scanner public-code /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml xdg-decoration-protocol.c

file(GLOB_RECURSE WAYLAND_CLIENT_APPLICATION_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)

add_executable(
    WayClient
    ${WAYLAND_CLIENT_APPLICATION_SOURCES}
)

# cc -o client main.c xdg-shell-protocol.c xdg-decoration-protocol.c -lwayland-client -lrt -lxkbcommon
target_link_libraries(WayClient PRIVATE wayland-client wayland-egl EGL rt xkbcommon m)
