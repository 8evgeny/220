cmake_minimum_required(VERSION 3.16)

project(qgst-goen220 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MAKE_PROGRAM "make -j${nproc}") # Число потоков компиляции.

#add_compile_options(-fopenmp )

find_package(QT NAMES Qt6 #[[Qt5]] REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core LinguistTools)

find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS DBus)

# Найти OpenCL
#find_package(OpenCL REQUIRED )
find_package(OpenMP REQUIRED   )

include_directories(${OpenCL_INCLUDE_DIRS})

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

#set( CMAKE_SYSROOT "/home/projects/goen-220/sysroot" )
set( CMAKE_SYSROOT "" )

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

#find_path(GLIBCFG_INCLUDE_DIR glibconfig.h
#    PATHS ${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu/glib-2.0/include
#    DOC "GLIB include directory")

find_library(GSTREAMER_LIBRARY
    NAMES gstreamer-1.0
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

#if(NOT GSTREAMER_INCLUDE_DIR OR NOT GSTREAMER_LIBRARY OR NOT GSTBASE_LIBRARY OR NOT LIBGOBJECT_LIBRARY)
#    message(FATAL_ERROR "GStreamer components not found")
#endif()

set(USE_COLOR_CHANNEL_MODE 8UC3)
# Использование GPU:
set(USE_GPU 2) # (0 - CPU, 1 - CUDA, 2 - OPENCL)

# Тип корреляции при template matching для USE_CORR_TYPE:
# CORR_TYPE = 2 : SQDIFF_NORMED (Прямая корреляция)
# CORR_TYPE = 6 : CCOEFF_NORMED  (Кросскорреляция модифицированная)
# CORR_TYPE = 8 ; COMBINED (объединение прямой и модифицированной кросскорреляции)
# CORR_TYPE = 10 : TKDNN, RKNN
set(USE_CORR_TYPE 8)

# Использование версии Yolo v3, v4 или trt:
set(USE_YOLO_VERSION trt)
set(TKDNN_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../../tkDNN)
#################################################################################################

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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/khadas/QtProg/220/Debug") # директория сборки проекта
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "/home/khadas/QtProg/220/lib")

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
    endif() # END if(${USE_CORR_TYPE} EQUAL 10)
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

find_package(OpenCV REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)

if( GYRO_PLATFORM_HOST_ENABLE)
    set(	GYRO_PLATFORM_HANDLER_SRC
                gpf/GyroPlatformHandler.cpp
                gpf/GyroPlatformHandler.h
                gpf/Pid.cpp
                gpf/Pid.h)
endif()

add_executable(qgst-goen220
  main.cpp
  cmd_parser.cpp
  cmd_parser.h
  main_app.cpp
  main_app.h
  network.cpp
  network.h
  settingsmanager.cpp
  settingsmanager.h
  telemetry.cpp
  telemetry.h
  ldc20i/ldc20i_controller.cpp
  ldc20i/ldc20i_controller.h
  ldc20i/ldc20i_qserial.cpp
  ldc20i/ldc20i_qserial.h
  ldc20i/ldc20i_worker.cpp
  ldc20i/ldc20i_worker.h

#  lib/lib_trac_tmml_8_CL.so

  mini640/mini640_controller.cpp
  mini640/mini640_controller.h
  mini640/mini640_qserial.cpp
  mini640/mini640_qserial.h
  mini640/mini640_worker.cpp
  mini640/mini640_worker.h
  ev9500m/visca/libvisca.cpp
  ev9500m/visca/libvisca.h
  ev9500m/visca/libvisca_posix.cpp
  ev9500m/camera_ev9500m.cpp
  ev9500m/camera_ev9500m.h
  ev9500m/camera_ev9500m_worker.cpp
  ev9500m/camera_ev9500m_worker.h
  j200/lens_j200.cpp
  j200/lens_j200.h
  j200/lens_j200_controller.cpp
  j200/lens_j200_controller.h
  j200/lens_j200_wrapper.h
  j200/lens_j200_wrapper.cpp
  gst_wrapper.cpp
  gst_wrapper.h
  j200/lens_j200_worker.cpp
  j200/lens_j200_worker.h
# gpf/gpf_controller.cpp gpf/gpf_controller.h gpf/gpf_qserial.cpp gpf/gpf_qserial.h gpf/gpf_worker.cpp gpf/gpf_worker.h
# gpf/GyroPlatformHandler.cpp
# gpf/GyroPlatformHandler.h
  ${GYRO_PLATFORM_HANDLER_SRC}
  vdb_registers.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${GSTREAMER_INCLUDE_DIR}
    ${GLIB_INCLUDE_DIR}
    ${GLIBCFG_INCLUDE_DIR}
    ${OCV_INCLUDE_DIR}
)

file(GLOB TracShatsFiles "${CMAKE_CURRENT_LIST_DIR}/tracshats/*")

#message("---CMAKE_CURRENT_LIST_DIR:" ${CMAKE_CURRENT_LIST_DIR}/lib)

link_directories(${CMAKE_CURRENT_LIST_DIR}/lib)

target_link_libraries(qgst-goen220 Qt${QT_VERSION_MAJOR}::Core
    OpenMP::OpenMP_CXX
    Qt6::Network Qt6::DBus
    OpenCL::OpenCL
    Qt6::SerialPort
    ${OpenCL_LIBRARIES}
    ${OpenCV_LIBS}
    ${GSTREAMER_LIBRARY}
    ${GLIB_LIBRARY}
    ${LIBDBUS_LIBRARY}
    ${LIBGOBJECT_LIBRARY}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Debug/gst/libfpsmonitor.so
    ${CMAKE_CURRENT_SOURCE_DIR}/../Debug/gst/libgoentrack.so
    ${CMAKE_CURRENT_SOURCE_DIR}/../prebuild/lib_trac_tmml_8_CL.so

    ${GYRO_PLATFORM_HOST_LIB_NAME}
)
#    -L"${CMAKE_BINARY_DIR}/../../build/gst/"  ${trac_lib_name}

function(copy_qt_libraries target)
    # Get linked Qt libraries
    get_target_property(linked_libs ${target} LINK_LIBRARIES)

    # Find Qt library paths
    foreach(lib ${linked_libs})
        if(lib MATCHES "Qt[0-9]::")
            get_target_property(lib_path ${lib} LOCATION)
            if(lib_path)
                add_custom_command(TARGET ${target} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${lib_path}"
                        "${CMAKE_INSTALL_LIBDIR}/qt_libs/"
                    COMMENT "Copying Qt library: ${lib_path}"
                )
            endif()
        endif()
    endforeach()

    # Also copy Qt plugins if needed
    # set(qt_plugins platforms imageformats sqldrivers)
    # foreach(plugin ${qt_plugins})
    #     get_target_property(qt_dir Qt6::Core LOCATION)
    #     get_filename_component(qt_dir ${qt_dir} DIRECTORY)
    #     set(plugin_dir "${qt_dir}/../plugins/${plugin}")

    #     if(EXISTS ${plugin_dir})
    #         add_custom_command(TARGET ${target} POST_BUILD
    #             COMMAND ${CMAKE_COMMAND} -E copy_directory
    #                 "${plugin_dir}"
    #                 "${CMAKE_CURRENT_BINARY_DIR}/qt_libs/${plugin}"
    #             COMMENT "Copying Qt plugins: ${plugin}"
    #         )
    #     endif()
    # endforeach()
endfunction()

# Usage
#copy_qt_libraries(qgst-goen220)

include(GNUInstallDirs)
install(TARGETS qgst-goen220
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
