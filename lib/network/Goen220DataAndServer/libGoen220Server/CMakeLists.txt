cmake_minimum_required(VERSION 3.16)

project(libGoen220Server LANGUAGES CXX)

add_compile_options(-fpermissive)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)


set(src_dir src)

set(src_list
    ${src_dir}/libgoen220_server.cpp
    ${src_dir}/libgoen220_server.h
    ${src_dir}/_internal/tcp_server.h
    ${src_dir}/_internal/tcp_server.cpp
    ${src_dir}/_internal/server_impl.h
    ${src_dir}/_internal/server_impl.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${src_list})

add_library(libGoen220Server STATIC ${src_list})

set_property(TARGET libGoen220Server PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(libGoen220Server PUBLIC src)

target_link_libraries(libGoen220Server PUBLIC libGoen220Data)

target_link_libraries(libGoen220Server PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
)

target_compile_definitions(libGoen220Server PRIVATE LIBGOEN220_SERVER_LIBRARY)
