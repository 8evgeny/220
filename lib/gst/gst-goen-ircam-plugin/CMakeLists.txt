cmake_minimum_required(VERSION 3.10)
project(libgray16togray8 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCL REQUIRED )
include_directories(${OpenCL_INCLUDE_DIRS})

set(USE_CUDA_RESIZE_DEVICE_IMX219 0) # использование resize через cv::cuda на уровне device
set(SYSTEM_ENDIANNESS LITTLE) # системный порядок байтов LITTLE / BIG (зачастую LITTLE).

set(OpenCV_DIR "/usr/include/opencv4")

list(APPEND CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/opencv4")
find_package(OpenCL REQUIRED )

find_package(OpenMP REQUIRED   )

message(STATUS "Found OpenCV: ${OpenCV_VERSION}")
message(STATUS "OpenCV Libraries: ${OpenCV_LIBS}")

# Enable NEON flags for ARM
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|ARM|aarch64|AARCH64")
    add_compile_options(-mfpu=neon -mfloat-abi=hard)
    message(STATUS "NEON flags enabled: -mfpu=neon -mfloat-abi=hard")
endif()

# GStreamer configuration
find_path(GSTREAMER_INCLUDE_DIR gst/gst.h
    PATHS ${CMAKE_SYSROOT}/usr/include/gstreamer-1.0
          ${CMAKE_SYSROOT}/usr/local/include/gstreamer-1.0
    DOC "GStreamer include directory")

find_path(GLIB_INCLUDE_DIR glib.h
    PATHS ${CMAKE_SYSROOT}/usr/include/glib-2.0
          ${CMAKE_SYSROOT}/usr/local/include/glib-2.0
          ${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu/glib-2.0
    DOC "GLIB include directory")

find_path(GLIBCFG_INCLUDE_DIR glibconfig.h
    PATHS ${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu/glib-2.0/include
           ${CMAKE_SYSROOT}/usr/lib/x86_64-linux-gnu/glib-2.0/include/
    DOC "GLIB include directory")

find_library(GSTREAMER_LIBRARY
    NAMES gstreamer-1.0
    PATHS ${CMAKE_SYSROOT}/usr/lib ${CMAKE_SYSROOT}/usr/local/lib
    DOC "GStreamer main library")

find_library(GSTVIDEO_LIBRARY
    NAMES gstvideo-1.0
    PATHS ${CMAKE_SYSROOT}/usr/lib ${CMAKE_SYSROOT}/usr/local/lib
    DOC "GStreamer main library")

find_library(GSTBASE_LIBRARY
    NAMES gstbase-1.0
    PATHS ${CMAKE_SYSROOT}/usr/lib ${CMAKE_SYSROOT}/usr/local/lib
    DOC "GStreamer base library")

find_library(GLIB_LIBRARY
    NAMES glib-2.0
    PATHS ${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu/ ${CMAKE_SYSROOT}/usr/local/lib
    DOC "glib-2.0 library")

find_library(LIBGOBJECT_LIBRARY
    NAMES gobject-2.0
    PATHS ${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu ${CMAKE_SYSROOT}/usr/local/lib
    DOC "libgobject-2.0 library")

find_library(LIBDBUS_LIBRARY
    NAMES dbus-1
    PATHS ${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu ${CMAKE_SYSROOT}/usr/local/lib
    DOC "dbus-1 library")

find_library(LIBGIO_LIBRARY
    NAMES gio-2.0
    PATHS ${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu ${CMAKE_SYSROOT}/usr/local/lib
    DOC "gio-2.0 library")

# find_library(LIBOMP_LIBRARY
#     NAMES libgomp1
#     PATHS ${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu ${CMAKE_SYSROOT}/usr/local/lib
#     DOC " omp gomp iomp5 library"
#     REQUIRED)

if(NOT GSTREAMER_INCLUDE_DIR OR NOT GSTREAMER_LIBRARY OR NOT GSTBASE_LIBRARY OR NOT LIBGOBJECT_LIBRARY)
    message(FATAL_ERROR "GStreamer components not found")
endif()

set(CMAKE_MAKE_PROGRAM "make -j${nproc}") # Число потоков компиляции.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(LIST_VALID_COLOR_CHANNEL_MODES 8UC1 8UC2 8UC3 8UC4)

add_library(${PROJECT_NAME} MODULE
    g16g8_ir.cpp
    g16g8_ir.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
     LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../../Debug/gst"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${GSTREAMER_INCLUDE_DIR}
    ${GLIB_INCLUDE_DIR}
    ${GLIBCFG_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
    ${GSTBASE_LIBRARY}
    ${GSTREAMER_LIBRARY}
    ${GSTVIDEO_LIBRARY}
    ${LIBGIO_LIBRARY}
    ${LIBOMP_LIBRARY}
    ${LIBDBUS_LIBRARY}
    OpenMP::OpenMP_CXX
)

message(" OCV " ${OpenCV_INCLUDE_DIRS} " LIB:"    ${OpenCV_LIBS})

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
