cmake_minimum_required(VERSION 3.10)
project(libfpsmonitor)
set(CMAKE_CXX_STANDARD 17)
#include_directories(${OpenCL_INCLUDE_DIRS})

set(CMAKE_MAKE_PROGRAM "make -j${nproc}") # Число потоков компиляции.

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

if(NOT GSTREAMER_INCLUDE_DIR OR NOT GSTREAMER_LIBRARY OR NOT GSTBASE_LIBRARY OR NOT LIBGOBJECT_LIBRARY)
    message(FATAL_ERROR "GStreamer components not found")
endif()

set(CMAKE_MAKE_PROGRAM "make -j${nproc}") # Число потоков компиляции.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${PROJECT_NAME} MODULE
    fpsmonitor.cpp
    fpsmonitor.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "/media/user/WORK/Github/Sublime_tests/Current/goen_220/Debug/gst"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${GSTREAMER_INCLUDE_DIR}
    ${GLIB_INCLUDE_DIR}
    ${GLIBCFG_INCLUDE_DIR}
)
message(" OCV " ${OpenCV_INCLUDE_DIRS} " LIB:"    ${OpenCV_LIBS})

target_link_libraries(${PROJECT_NAME}
    ${GSTBASE_LIBRARY}
    ${GSTREAMER_LIBRARY}
    ${GSTVIDEO_LIBRARY}
    ${LIBGIO_LIBRARY}
    ${LIBDBUS_LIBRARY}
)

#include(GNUInstallDirs)
#install(TARGETS ${PROJECT_NAME}
#        DESTINATION "/media/user/WORK/Github/Sublime_tests/Current/goen_220/Debug/gst"
#)
#LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
