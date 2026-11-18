cmake_minimum_required(VERSION 3.16)

project(drone C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)

add_executable(drone drone.c)

target_link_libraries(drone
    PRIVATE
        PkgConfig::SDL2
        m
)
