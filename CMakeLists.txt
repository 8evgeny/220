cmake_minimum_required(VERSION 3.14)
project(qgst-goen220-daemon)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/Debug")
set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "" FORCE)
message(STATUS "Installation will go to: ${CMAKE_INSTALL_PREFIX}")

set(CMAKE_AR "/usr/bin/aarch64-linux-gnu-ar")
set(CMAKE_RANLIB "/usr/bin/aarch64-linux-gnu-ranlib")

set( GYRO_PLATFORM_HOST_ENABLE true)

add_compile_options(-fopenmp )

set(OpenCV_DIR $ENV{HOME}/opencv/install_4_7_0_contrib)

find_package(Qt6 REQUIRED COMPONENTS Core SerialPort Network DBus
    PATHS "${CMAKE_PREFIX_PATH}"
    NO_DEFAULT_PATHcd 
    NO_CMAKE_FIND_ROOT_PATH
)

find_package(OpenMP REQUIRED   )
find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)
find_package(OpenCL REQUIRED )

if( GYRO_PLATFORM_HOST_ENABLE)
    add_compile_definitions( GYRO_PLATFORM_HOST)
#    add_compile_definitions( GYRO_PLATFORM_USE_DEBUG_CONFIG)    #   Подмена config.ini на отладочный конфиг. Для отладки!
    set( GYRO_PLATFORM_HOST_LIB_NAME Goen220GyroPlatformHost)
endif()

include_directories(
    "${CMAKE_CURRENT_LIST_DIR}/prebuild/"
    "/usr/lib/x86_64-linux-gnu/glib-2.0/include"
    )

 # add_subdirectory( lib/gst/gst-goen-ircam-plugin)
 add_subdirectory( lib/gst/gst-goen-fps-mon-plugin)
 add_subdirectory( lib/gst/gst-goen-track-plugin)
 add_subdirectory( app)

# add_subdirectory( lib/network/Goen220DataAndServer/libGoen220Data)
# add_subdirectory( lib/network/Goen220DataAndServer/libGoen220Server)

if(GYRO_PLATFORM_HOST_ENABLE)
    add_subdirectory( lib/gsp/Goen220Vdb)
    add_subdirectory( lib/gsp/Goen220GyroPlatformHost)
endif()
