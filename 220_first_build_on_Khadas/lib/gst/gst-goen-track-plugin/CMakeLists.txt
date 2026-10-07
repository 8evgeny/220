cmake_minimum_required(VERSION 3.10)
project(libgoentrack)
set(CMAKE_CXX_STANDARD 17)
set(OpenCV_DIR $ENV{HOME}/opencv/install_4_7_0_contrib)
find_package(OpenCL REQUIRED )
find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)

set(CMAKE_MAKE_PROGRAM "make -j${nproc}") # Число потоков компиляции.

#include_directories(${OpenCL_INCLUDE_DIRS})

# Использование GPU:
set(USE_GPU 2) # (0 - CPU, 1 - CUDA, 2 - OPENCL)

# Тип корреляции при template matching для USE_CORR_TYPE:
# CORR_TYPE = 2 : SQDIFF_NORMED (Прямая корреляция)
# CORR_TYPE = 6 : CCOEFF_NORMED  (Кросскорреляция модифицированная)
# CORR_TYPE = 8 ; COMBINED (объединение прямой и модифицированной кросскорреляции)
# CORR_TYPE = 10 : TKDNN, RKNN
set(USE_CORR_TYPE 8)

# Работа в режиме GUI (медленно), или без GUI (быстро):
set(USE_GUI 0)
# Использование логгера
set(USE_LOGGER 1)
# Режим цветового пространства, получаемых изображений от источника видеопотока
set(USE_COLOR_CHANNEL_MODE 8UC3) # (8UC3 -- BGR, 8UC1 -- GRAY8, 8UC4 -- RGBA)

set(CV_GST_SEND 0)      # tool - Sending video stream to any HOST  section [cv_gst_send] in inifile
set(MAT2RTSP 0) # tool - Sending video stream to RTSP server
set(USE_ROI_TMP 0) # Работа в ROI и с центральным шаблоном.

# Use Devices:
set(USE_FOLDER_READER 0) # 0
set(USE_VIDEO_READER 0) # 1
set(USE_WEB_CAMERA 0) # 2
set(USE_COLIBRI_TV 0) # 3
set(USE_RASPBERRY_HQ_CAMERA 0) # 4
set(USE_IMX477_SQUARE_CAMERA_MIPI 0) # 5
set(USE_IMX219_CAMERA_MIPI 0) # 6
set(USE_HIKVISION 0) # 7
set(USE_SHARED_MEMORY 0) # 8
set(USE_HVGS_GRAY_USB_CAMERA 0) # 9
set(USE_CORSAIR_400_RAW 0) # 10 (Шар ОЭЦН400, сырой UDP видеопоток напрямую)
set(USE_IMX415_CAMERA_MIPI 0) # 11
set(USE_RTSP_SERVER 0) # 12
set(USE_ETHERNET_CONNECTION 0)
set(USE_PCAP 0)

set(USE_CUDA_RESIZE_DEVICE_IMX219 0) # использование resize через cv::cuda на уровне device
set(SYSTEM_ENDIANNESS LITTLE) # системный порядок байтов LITTLE / BIG (зачастую LITTLE).

set(OpenCV_DIR "/usr/include/opencv4")

list(APPEND CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/opencv4")
find_package(OpenCL REQUIRED )
set(OpenCV_DIR $ENV{HOME}/opencv/install_4_7_0_contrib)
find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)

#set(OpenCV_INCLUDE_DIRS   "/usr/include/opencv4")
message(STATUS "Found OpenCV: ${OpenCV_VERSION}")
message(STATUS "OpenCV Libraries: ${OpenCV_LIBS}")

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

# include_directories(
#     ${GSTREAMER_INCLUDE_DIR}
#     ${GLIB_INCLUDE_DIR}
#     ${GLIBCFG_INCLUDE_DIR}
#     ${OCV_INCLUDE_DIR}

# )


# link_directories(
#     ${GSTREAMER_LIBRARY_DIRS}
#     ${OPENCV_LIBRARY_DIRS}
#     ${DBUS_LIBRARY_DIRS}
#     ${OpenCL_LIBRARIES}

# )

if(${USE_GPU} EQUAL 0)
    set(GPU_PREFIX cpu)
    message(STATUS "--------------------------- USE_CPU")
endif() # END if(${USE_GPU} EQUAL 0)

if(${USE_GPU} EQUAL 1)
    if(${USE_CORR_TYPE} EQUAL 10)
        set(GPU_PREFIX gpu)
        if(${USE_YOLO_VERSION} STREQUAL trt)
            set(GPU_LIB -ltkdnn_yolo)
        endif() # END elseif(${USE_YOLO_VERSION} STREQUAL trt)
        link_directories(${TKDNN_DIRECTORY})
        add_definitions(-DYOLO_${USE_YOLO_VERSION})
        message(STATUS "--------------------------- USE_YOLO_${USE_YOLO_VERSION}")
    endif() # END if(${USE_CORR_TYPE} EQUAL 10)
endif() # END if(${USE_GPU} EQUAL 1)

if(${USE_GPU} EQUAL 2)
    include_directories($CMAKE_SOURCE_DIR})
    find_package(OpenCL REQUIRED)
    include_directories (${OpenCL_INCLUDE_DIRS})
    set(GPU_LIB ${OpenCL_LIBRARY})
endif() # END if(${USE_GPU} EQUAL 2)

set(CMAKE_MAKE_PROGRAM "make -j${nproc}") # Число потоков компиляции.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/run) # директория сборки проекта

set(LIST_VALID_COLOR_CHANNEL_MODES 8UC1 8UC2 8UC3 8UC4)

if(NOT ${USE_COLOR_CHANNEL_MODE} IN_LIST LIST_VALID_COLOR_CHANNEL_MODES)
    message("Note: valid COLOR_CHANNEL_MODE list: " "${LIST_VALID_COLOR_CHANNEL_MODES}")
    message(FATAL_ERROR "Error: not recognize \"COLOR_CHANNEL_MODE\" mode = \"${USE_COLOR_CHANNEL_MODE}\"")
else()
    message(STATUS "USE_COLOR_CHANNEL_MODE = ${USE_COLOR_CHANNEL_MODE}")
    add_definitions(-DCCM_${USE_COLOR_CHANNEL_MODE})
endif()

if(${USE_GPU} EQUAL 0)
    set(trac_lib_name -l_trac_tmml_${USE_CORR_TYPE}_CPU) # Название подключаемой библиотеки CPU
    add_definitions(-DUSE_CPU)
    message(STATUS "--------------------------- USE_CORR_TYPE=${USE_CORR_TYPE}")
    message(STATUS "--------------------------- USE_CPU")
    add_definitions(-Dclass_name=trac_tmml)
    add_definitions(-Dcreate_name=create_track)
endif() # END if(${USE_GPU} EQUAL 0)

if(${USE_GPU} EQUAL 1)
    if(NOT ${USE_CORR_TYPE} EQUAL 10)
        set(trac_lib_name -l_trac_tmml_${USE_CORR_TYPE}_CUDA) # Название подключаемой библиотеки CUDA
        add_definitions(-Dclass_name=trac_tmml)
        add_definitions(-Dcreate_name=create_track)
    endif()
    if(${USE_CORR_TYPE} EQUAL 10)
        set(trac_lib_name -l_trac_tmml_TKDNN) # Название подключаемой библиотеки CUDA
        add_definitions(-DTKDNN)
        message(STATUS "--------------------------- USE_TKDNN")
        add_definitions(-Dclass_name=nnet)
        add_definitions(-Dcreate_name=create_nnet)
    endif() # END   if(${USE_CORR_TYPE} EQUAL 10)
    add_definitions(-DUSE_CUDA)
    message(STATUS "--------------------------- USE_CORR_TYPE=${USE_CORR_TYPE}")
    message(STATUS "--------------------------- USE_CUDA")
endif() # END if(${USE_GPU} EQUAL 1)

if(${USE_GPU} EQUAL 2)
    set(trac_lib_name -l_trac_tmml_${USE_CORR_TYPE}_CL) # Название подключаемой библиотеки CL
    if(NOT ${USE_CORR_TYPE} EQUAL 10)
       add_definitions(-DUSE_CL)
       message(STATUS "--------------------------- USE_CORR_TYPE=${USE_CORR_TYPE}")
       message(STATUS "--------------------------- USE_CL")
       add_definitions(-Dclass_name=trac_tmml)
       add_definitions(-Dcreate_name=create_track)
    endif() # END if(NOT ${USE_CORR_TYPE} EQUAL 10)
    if(${USE_CORR_TYPE} EQUAL 10)
        set(trac_lib_name -l_trac_tmml_RKNN) # Название подключаемой библиотеки CL
        add_definitions(-DTKDNN)
        message(STATUS "--------------------------- USE_CORR_TYPE=${USE_CORR_TYPE}")
        message(STATUS "--------------------------- USE_RKNN")
        add_definitions(-Dclass_name=nnet)
        add_definitions(-Dcreate_name=create_nnet)
    endif() # END if(${USE_CORR_TYPE} EQUAL 10)
endif() # END if(${USE_GPU} EQUAL 2)

if(${USE_ROI_TMP} EQUAL 1)
    add_definitions(-DUSE_ROI_TMP)
endif()

add_library(${PROJECT_NAME} MODULE
    tracshats/trac_api.hpp
    tracshats/trac_struct.hpp
    main_app.h main_app.cpp
    trsh_wrapper.cpp
    trsh_wrapper.h
    gstgoentrack.h
    gstgoentrack_cb.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${GSTREAMER_INCLUDE_DIR}
    ${GLIB_INCLUDE_DIR}
    ${GLIBCFG_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")

target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
    ${GSTBASE_LIBRARY}
    ${GSTREAMER_LIBRARY}
    ${GSTVIDEO_LIBRARY}
    ${LIBGIO_LIBRARY}
    ${LIBDBUS_LIBRARY}
)

message(" OCV " ${OpenCV_INCLUDE_DIRS} " LIB:"    ${OpenCV_LIBS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../../Debug/gst"
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
