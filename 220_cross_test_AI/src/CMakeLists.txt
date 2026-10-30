cmake_minimum_required(VERSION 3.16)

project(qgst-goen220-daemon)

include(CTest)

get_filename_component(GOEN220_SOURCE_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(GOEN220_PREBUILD_DIR "${GOEN220_SOURCE_ROOT}/prebuild")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "Installation directory" FORCE)
endif()
message(STATUS "Installation will go to: ${CMAKE_INSTALL_PREFIX}")

add_custom_target(copy_prebuild_dependencies ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${GOEN220_PREBUILD_DIR}"
        "${CMAKE_BINARY_DIR}"
    COMMENT "Copying prebuild dependencies to build directory"
    VERBATIM
)

option(GYRO_PLATFORM_HOST_ENABLE "Build with the host gyro platform" ON)

find_package(Qt6 REQUIRED COMPONENTS Core SerialPort Network DBus)
find_package(OpenCV REQUIRED)

#set(CMAKE_MAKE_PROGRAM "make -j${nproc}") # Число потоков компиляции.

if(GYRO_PLATFORM_HOST_ENABLE)
    add_compile_definitions(GYRO_PLATFORM_HOST)
#    add_compile_definitions(GYRO_PLATFORM_USE_DEBUG_CONFIG)    #   Подмена config.ini на отладочный конфиг. Для отладки!
    set(GYRO_PLATFORM_HOST_LIB_NAME Goen220GyroPlatformHost)
endif()

add_subdirectory(app)

add_subdirectory(lib/gst/gst-goen-fps-mon-plugin)

add_subdirectory(lib/gst/gst-goen-ircam-plugin)

add_subdirectory(lib/gst/gst-goen-track-plugin)

# add_subdirectory( lib/network/Goen220DataAndServer/libGoen220Data)
# add_subdirectory( lib/network/Goen220DataAndServer/libGoen220Server)

if(GYRO_PLATFORM_HOST_ENABLE)
    add_subdirectory(lib/gsp/Goen220Vdb)
    add_subdirectory(lib/gsp/Goen220GyroPlatformHost)
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
