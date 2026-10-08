cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

project(trac_from_lib_tmml)
message(STATUS "")
message(STATUS "-------------------------------- CMAKE BEGINING for project trac_from_lib_tmml")

# Использование GPU:
set(USE_GPU 2) # (0 - CPU, 1 - CUDA, 2 - OPENCL)

# Тип корреляции при template matching для USE_CORR_TYPE:
# CORR_TYPE = 2 : SQDIFF_NORMED (Прямая корреляция)
# CORR_TYPE = 6 : CCOEFF_NORMED  (Кросскорреляция модифицированная)
# CORR_TYPE = 8 ; COMBINED (объединение прямой и модифицированной кросскорреляции)
set(USE_CORR_TYPE 8)

set(trt_yolo 0) # 0 - без NNET, 1 - TKDNN, 2 - YOLO, 3 - RKNN.

# Работа в режиме GUI (медленно), или без GUI (быстро):
set(USE_GUI 1)
# Использование логгера
set(USE_LOGGER 1)
# Режим цветового пространства, получаемых изображений от источника видеопотока
set(USE_COLOR_CHANNEL_MODE 8UC3) # (8UC3 -- BGR, 8UC1 -- GRAY8, 8UC4 -- RGBA)

set(CV_GST_SEND 0)      # tool - Sending video stream to any HOST  section [cv_gst_send] in inifile
set(MAT2RTSP 0) # tool - Sending video stream to RTSP server
set(USE_ROI_TMP 0) # Работа в ROI и с центральным шаблоном.

# Use Devices:
set(USE_FOLDER_READER 1) # 0
set(USE_VIDEO_READER 0) # 1
set(USE_WEB_CAMERA 1) # 2
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

#################################################################################################
if(${trt_yolo} EQUAL 3)
    set(OpenCV_DIR $ENV{HOME}/opencv/install_4_10_0_contrib)
else()
   set(OpenCV_DIR $ENV{HOME}/opencv/install_4_10_0_contrib_cuda)
endif()

if(${USE_CORSAIR_400_RAW} EQUAL 1)
    set(DBG_VIDEO_SENDER 0) # включение отправки видео и командного обмена с эмулятором шара (QtProg/poes400/corsair_terminal_emulator)
    # Используем программу в паре с терминалкой 400-го, раздел ini-файла [vtrack_data_sender], подключаем библиотеку vtrack_data_sender
    set(USE_DBG_SEND_VTRACK 0) # дополнителоное логирование отладочной информации в процессе отправки рамок целей
endif() #     if(${USE_CORSAIR_400_RAW} EQUAL 1)

# Использование версии Yolo v3, v4 или trt:
set(TKDNN_DIRECTORY $ENV{HOME}/QtProg/tkDNN)
#################################################################################################

if(${USE_GUI} EQUAL 0)
    set(GUI_RUN_PSTFX "notgui")
elseif(${USE_GUI} EQUAL 1)
    set(GUI_RUN_PSTFX "gui")
else()
    message(FATAL_ERROR "Error: incorrect \"USE_GUI\" parameter = \"#{USE_GUI}\"")
endif() # END if(NOT ${USE_GUI} EQUAL 0)

if(${USE_GPU} EQUAL 0)    
    add_definitions(-DUSE_CPU)

    if(${trt_yolo} EQUAL 0)
        set(trac_lib_name -l_trac_tmml_${USE_CORR_TYPE}_CPU)
        set(GPU_LIB -ldarknet_cpu_v4)
        link_directories(lib_darknet)
        add_definitions(-DUSE_TRAC)
        set(run_name run_${GUI_RUN_PSTFX}_${USE_COLOR_CHANNEL_MODE}_${USE_CORR_TYPE}_CPU)
    endif() # END if(${trt_yolo} EQUAL 0)

    if(${trt_yolo} EQUAL 2)
        set(trac_lib_name -l_trac_tmml_YOLO)
        set(GPU_LIB -ldarknet_cpu_v4)
        link_directories(lib_darknet)
        add_definitions(-DUSE_NNET)
        set(run_name run_${GUI_RUN_PSTFX}_${USE_COLOR_CHANNEL_MODE}_YOLO_CPU)
    endif() # END if(${trt_yolo} EQUAL 2)
endif() # END if(${USE_GPU} EQUAL 0)

if(${USE_GPU} EQUAL 1)
        add_definitions(-DUSE_CUDA)

        if(${trt_yolo} EQUAL 0)
            set(trac_lib_name -l_trac_tmml_${USE_CORR_TYPE}_CUDA)
            set(run_name run_${GUI_RUN_PSTFX}_${USE_COLOR_CHANNEL_MODE}_${USE_CORR_TYPE}_CUDA)
            add_definitions(-DUSE_TRAC)
        endif() # END if(${trt_yolo} EQUAL 0)

        if(${trt_yolo} EQUAL 1)
            set(trac_lib_name -l_trac_tmml_TKDNN)
            set(GPU_LIB ${CUDA_LIBRARIES} ${CUDA_CUBLAS_LIBRARIES} ${CUDNN_LIBRARIES} -ltkDNN -lkernels -ltkdnn_yolo)
            link_directories(${TKDNN_DIRECTORY})
            add_definitions(-DUSE_NNET)
            set(run_name run_${GUI_RUN_PSTFX}_${USE_COLOR_CHANNEL_MODE}_TKDNN)
        endif() # END if(${trt_yolo} EQUAL 1)

        if(${trt_yolo} EQUAL 2)
            set(trac_lib_name -l_trac_tmml_YOLO)
            set(GPU_LIB -ldarknet_gpu_v4)
            link_directories(lib_darknet)
            add_definitions(-DUSE_NNET)
            set(run_name run_${GUI_RUN_PSTFX}_${USE_COLOR_CHANNEL_MODE}_YOLO)
        endif() # END if(${trt_yolo} EQUAL 2)
endif() # END if(${USE_GPU} EQUAL 1)

if(${USE_GPU} EQUAL 2)
    add_definitions(-DUSE_CL)

    include_directories($CMAKE_SOURCE_DIR})
    find_package(OpenCL REQUIRED)
    include_directories (${OpenCL_INCLUDE_DIRS})
    set(GPU_LIB ${OpenCL_LIBRARY})

    if(${trt_yolo} EQUAL 0)
        set(trac_lib_name -l_trac_tmml_${USE_CORR_TYPE}_CL) # Название подключаемой библиотеки CL
        add_definitions(-DUSE_TRAC)
        set(run_name run_${GUI_RUN_PSTFX}_${USE_COLOR_CHANNEL_MODE}_${USE_CORR_TYPE}_CL)
    endif() # END if(${trt_yolo} EQUAL 0)

    if(${trt_yolo} EQUAL 3)
        set(trac_lib_name -l_trac_tmml_RKNN) # Название подключаемой библиотеки CL
        add_definitions(-DUSE_NNET)
        set(run_name run_${GUI_RUN_PSTFX}_${USE_COLOR_CHANNEL_MODE}_RKNN)
    endif() # END if(${trt_yolo} EQUAL 3)
endif() # END if(${USE_GPU} EQUAL 2)

set(CMAKE_MAKE_PROGRAM "make -j${nproc}") # Число потоков компиляции.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/run) # директория сборки проекта
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib_modules)

set(LIST_VALID_COLOR_CHANNEL_MODES 8UC1 8UC2 8UC3 8UC4)

if(NOT ${USE_COLOR_CHANNEL_MODE} IN_LIST LIST_VALID_COLOR_CHANNEL_MODES)
    message("Note: valid COLOR_CHANNEL_MODE list: " "${LIST_VALID_COLOR_CHANNEL_MODES}")
    message(FATAL_ERROR "Error: not recognize \"COLOR_CHANNEL_MODE\" mode = \"${USE_COLOR_CHANNEL_MODE}\"")
else()
    message(STATUS "USE_COLOR_CHANNEL_MODE = ${USE_COLOR_CHANNEL_MODE}")
    add_definitions(-DCCM_${USE_COLOR_CHANNEL_MODE})
endif()


if(${USE_ROI_TMP} EQUAL 1)
    add_definitions(-DUSE_ROI_TMP)
endif()

if(${USE_GUI} EQUAL 1)
    add_definitions(-DGUI_OK)
    message(STATUS "--------------------------- USE_GUI=1")
else()
    set(SHOW_PSTFX )
    message(STATUS "--------------------------- USE_GUI=0")
endif()

link_directories(${CMAKE_CURRENT_LIST_DIR})

find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)
message(OpenCV_INCLUDE_DIRS=${OpenCV_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

link_directories(${CMAKE_CURRENT_LIST_DIR}/lib)

include_directories(tools/)

file(GLOB TracShatsFiles "${CMAKE_CURRENT_LIST_DIR}/tracshats/*")

add_executable(${CMAKE_PROJECT_NAME}
    tracking.ini
    main.cpp
    application.cpp
    application.hpp
    ##############  Trackers ###################################################
    ${TracShatsFiles}
    ##############  Tools  ####################################################
    tools/INIReader.h
    tools/ExtendINIReader.hpp
    tools/watchdog.h
    tools/colors.h
    tools/toolbox_common_utils/toolbox_common_utils.cpp
    tools/toolbox_common_utils/toolbox_common_utils.hpp
    tools/recorder/recorder.hpp
    tools/recorder/recorder.cpp
    tools/time_keeper/time_keeper.cpp
    tools/time_keeper/time_keeper.hpp
    tools/temperature_monitor/temperature_monitor.cpp
    tools/temperature_monitor/temperature_monitor.hpp
    tools/custom_filesystem/custom_filesystem.hpp # переименовать path_utils или files_os_tools/utils
    tools/custom_filesystem/custom_filesystem.cpp
    ################### Devices ################
    devices/device.cpp
    devices/device.hpp
    devices/iframehandler.cpp
    devices/iframehandler.h
    devices/icommandhandler.h
    )

if(${USE_LOGGER} EQUAL 1)
    add_definitions(-DUSE_LOGGER)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        logger/logger.cpp
        logger/logger.hpp
        logger/factory.cpp
        logger/factory.h
        logger/settings.hpp)
endif() # END if(${USE_LOGGER} EQUAL 1)

########################### BEGIN DEVICES:

if(${CV_GST_SEND} EQUAL 1)
    add_definitions(-DCV_GST_SEND)
    include_directories(tools/cv_gst_send)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        tools/cv_gst_send/cv_gst_send.hpp
        tools/cv_gst_send/cv_gst_send.cpp
        )
endif() #if(${CV_GST_SEND} EQUAL 1)

if(${MAT2RTSP} EQUAL 1)
    add_definitions(-DMAT2RTSP)
    include_directories(tools/mat2rtsp)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        tools/mat2rtsp/mat2rtsp.hpp
        tools/mat2rtsp/mat2rtsp.cpp
        )
    add_subdirectory(${CMAKE_SOURCE_DIR}/tools/mat2rtsp/rtsp_server) #RTSP сервер
endif() #if(${MAT2RTSP} EQUAL 1)

if(${USE_FOLDER_READER} EQUAL 1) # 0
    add_definitions(-DUSE_FOLDER_READER)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/FolderReader/folderreader.h
        devices/FolderReader/folderreader.cpp
        )
endif() # END if(${USE_FOLDER_READER} EQUAL 1)

if(${USE_VIDEO_READER} EQUAL 1) # 1
    add_definitions(-DUSE_VIDEO_READER)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/VideoReader/videoreader.h
        devices/VideoReader/videoreader.cpp
        )
endif() # END if(${USE_VIDEO_READER} EQUAL 1)

if(${USE_WEB_CAMERA} EQUAL 1) # 2
    add_definitions(-DUSE_WEB_CAMERA)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/WebCamera/webcamera.hpp
        devices/WebCamera/webcamera.cpp
        )
endif() # END if(${USE_WEB_CAMERA} EQUAL 1)

if(${USE_COLIBRI_TV} EQUAL 1) # 3
    add_definitions(-DUSE_COLIBRI_TV)
    set(USE_ETHERNET_CONNECTION 1)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/colibritv/colibritv.cpp
        devices/colibritv/colibritv.h
        devices/colibritv/eth/colibritvclient.cpp
        devices/colibritv/eth/colibritvclient.h
        devices/colibritv/eth/colibritv_ethcontroller.cpp
        devices/colibritv/eth/colibritv_ethcontroller.h
        devices/colibritv/eth/colibritvserver.cpp
        devices/colibritv/eth/colibritvserver.h
        )
endif() # END if(${USE_COLIBRI_TV} EQUAL 1)

if(${USE_RASPBERRY_HQ_CAMERA} EQUAL 1) # 4
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GST REQUIRED gstreamer-1.0>=1.4
        gstreamer-sdp-1.0>=1.4
        gstreamer-video-1.0>=1.4
        gstreamer-app-1.0>=1.4
        )
    add_definitions(-DUSE_RASPBERRY_HQ_CAMERA)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/RaspberryHQCamera/raspberryhqcamera.cpp
        devices/RaspberryHQCamera/raspberryhqcamera.hpp
        devices/RaspberryHQCamera/factory.cpp
        devices/RaspberryHQCamera/factory.hpp
        devices/RaspberryHQCamera/settings.hpp
        )
    target_link_libraries(${CMAKE_PROJECT_NAME} ${GST_LIBRARIES} gstapp-1.0)
    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${GST_INCLUDE_DIRS})
endif() # END if(${USE_RASPBERRY_HQ_CAMERA} EQUAL 1)

if(${USE_IMX477_SQUARE_CAMERA_MIPI} EQUAL 1) # 5
    add_definitions(-DUSE_IMX477_SQUARE_CAMERA_MIPI)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/imx477squareCameraMIPI/imx477_square_camera_mipi_factory.hpp
        devices/imx477squareCameraMIPI/imx477_square_camera_mipi_factory.cpp
        devices/imx477squareCameraMIPI/imx477_square_camera_mipi.hpp
        devices/imx477squareCameraMIPI/imx477_square_camera_mipi.cpp
        devices/imx477squareCameraMIPI/imx477_square_camera_mipi_settings.hpp
        )
endif() # END if(${USE_IMX477_SQUARE_CAMERA_MIPI} EQUAL 1)

if(${USE_IMX415_CAMERA_MIPI} EQUAL 1) # 5
    add_definitions(-DUSE_IMX415_CAMERA_MIPI)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/imx415CameraMIPI/imx415_camera_mipi_factory.hpp
        devices/imx415CameraMIPI/imx415_camera_mipi_factory.cpp
        devices/imx415CameraMIPI/imx415_camera_mipi.hpp
        devices/imx415CameraMIPI/imx415_camera_mipi.cpp
        devices/imx415CameraMIPI/imx415_camera_mipi_settings.hpp
        )
endif() # END if(${USE_IMX415_CAMERA_MIPI} EQUAL 1)

if(${USE_IMX219_CAMERA_MIPI} EQUAL 1) # 6
    add_definitions(-DUSE_IMX219_CAMERA_MIPI)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/IMX219CameraMIPI/imx219_camera_mipi_factory.hpp
        devices/IMX219CameraMIPI/imx219_camera_mipi_factory.cpp
        devices/IMX219CameraMIPI/imx219_camera_mipi.hpp
        devices/IMX219CameraMIPI/imx219_camera_mipi.cpp
        devices/IMX219CameraMIPI/camera_settings.hpp
        )
    if(${USE_CUDA_RESIZE_DEVICE_IMX219} EQUAL 1)
        add_definitions(-DUSE_CUDA_RESIZE_DEVICE_IMX219)
    endif() # END if(${USE_CUDA_RESIZE_DEVICE_IMX219} EQUAL 1)
endif() # END if(${USE_IMX219_CAMERA_MIPI} EQUAL 1)

if(${USE_HIKVISION} EQUAL 1) # 7
    add_definitions(-DUSE_HIKVISION)
    include_directories(tools/)
    include_directories(tools/toolbox_common_utils/)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/EthCameraRtsp/rtsp_h265_device.hpp
        devices/EthCameraRtsp/rtsp_h265_device.cpp
        devices/EthCameraRtsp/rtsp_h265_device_factory.hpp
        devices/EthCameraRtsp/rtsp_h265_device_factory.cpp
        devices/EthCameraRtsp/rtsp_h265_device_settings.hpp
        )
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GST REQUIRED gstreamer-1.0>=1.4
        gstreamer-sdp-1.0>=1.4
        gstreamer-video-1.0>=1.4
        gstreamer-app-1.0>=1.4
        gstrtp-1.0
        glib-2.0
        )
    target_link_libraries(${CMAKE_PROJECT_NAME} ${GST_LIBRARIES} ${GST_LIBRARIES} gstapp-1.0)
    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${GST_INCLUDE_DIRS})
endif() # END if(${USE_HIKVISION} EQUAL 1)

if(${USE_SHARED_MEMORY} EQUAL 1) # 8
    add_definitions(-DUSE_SHARED_MEMORY)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/SharedMemory/sharedmemory.cpp
        devices/SharedMemory/sharedmemory.hpp
        devices/SharedMemory/ipc/shared_memory.cpp
        devices/SharedMemory/ipc/shared_memory.h
        devices/SharedMemory/eth/sharedmemory_ethernetcontroller.cpp
        devices/SharedMemory/eth/sharedmemory_ethernetcontroller.h
        devices/SharedMemory/eth/sharedmemoryclient.cpp
        devices/SharedMemory/eth/sharedmemoryclient.hpp
        devices/SharedMemory/eth/sharedmemoryserver.cpp
        devices/SharedMemory/eth/sharedmemoryserver.hpp
        devices/SharedMemory/cmd/sharedmemory_cmdmanager.cpp
        devices/SharedMemory/cmd/sharedmemory_cmdmanager.hpp
        )
    target_link_libraries(${CMAKE_PROJECT_NAME} -lrt)
endif() # END if(${USE_SHARED_MEMORY} EQUAL 1)

if(${USE_HVGS_GRAY_USB_CAMERA} EQUAL 1) # 9
    add_definitions(-DUSE_HVGS_GRAY_USB_CAMERA)
    target_sources(devices PRIVATE
        devices/HVGS_GRAY_USB_Camera/hvgs_gray_usb_camera.hpp
        devices/HVGS_GRAY_USB_Camera/hvgs_gray_usb_camera.cpp
        devices/HVGS_GRAY_USB_Camera/hvgs_gray_usb_camera_factory.hpp
        devices/HVGS_GRAY_USB_Camera/hvgs_gray_usb_camera_factory.cpp
        devices/HVGS_GRAY_USB_Camera/hvgs_gray_usb_camera_settings.hpp
        tools/ExtendINIReader.hpp
        )
    add_library(lib_hvgs_camera SHARED IMPORTED)
    set_target_properties(lib_hvgs_camera PROPERTIES
        IMPORTED_LOCATION "/usr/lib/libMVSDK.so")
    target_link_libraries(devices lib_hvgs_camera)
endif() # END if(${USE_HVGS_GRAY_USB_CAMERA} EQUAL 1)

if(${USE_CORSAIR_400_RAW} EQUAL 1) # 10
    add_definitions(-DUSE_CORSAIR_400_RAW)
    set(cors400_device_dir_path "${CMAKE_CURRENT_LIST_DIR}/devices/Corsair400raw/")
    set(cors400_src_dir_path "${cors400_device_dir_path}/eth/")
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        ### device ###
        ${cors400_device_dir_path}/corsair400raw.hpp
        ${cors400_device_dir_path}/corsair400raw.cpp
        ${cors400_device_dir_path}/corsair400raw_factory.cpp
        ${cors400_device_dir_path}/corsair400raw_factory.hpp
        ${cors400_device_dir_path}/corsair400raw_settings.hpp
        ### corsair 400 eth platform ###
        #### base ####
        ${cors400_src_dir_path}/base/BaseThread.hpp
        ${cors400_src_dir_path}/base/BaseThread.cpp
        ${cors400_src_dir_path}/base/Socket.hpp
        ${cors400_src_dir_path}/base/Socket.cpp
        ${cors400_src_dir_path}/base/SocketAddress.hpp
        ${cors400_src_dir_path}/base/SocketAddress.cpp
        ${cors400_src_dir_path}/base/TripleBuffer.hpp
        ${cors400_src_dir_path}/base/TripleBuffer.cpp
        #### protocols ####
        ##### udp #####
        ${cors400_src_dir_path}/protocols/udp/UDPClient.hpp
        ${cors400_src_dir_path}/protocols/udp/UDPClient.cpp
        ${cors400_src_dir_path}/protocols/udp/UDPServer.hpp
        ${cors400_src_dir_path}/protocols/udp/UDPServer.cpp
        ${cors400_src_dir_path}/protocols/udp/UDPSocket.hpp
        ${cors400_src_dir_path}/protocols/udp/UDPSocket.cpp
        #### global ####
        ${cors400_src_dir_path}/EthGlobal.hpp
        #### servers ####
        ${cors400_src_dir_path}/servers/RAWVideoServer.hpp
        ${cors400_src_dir_path}/servers/RAWVideoServer.cpp
        ${cors400_src_dir_path}/servers/DataServer.hpp
        ${cors400_src_dir_path}/servers/DataServer.cpp
        ) # END target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    include_directories(
        ${cors400_device_dir_path}/
        ${cors400_src_dir_path}/
        ${cors400_src_dir_path}/base/
        ${cors400_src_dir_path}/protocols/udp/
        )
endif() # END if(${USE_CORSAIR_400_RAW} EQUAL 1)

if(${USE_RTSP_SERVER} EQUAL 1) # 12
    add_definitions(-DUSE_RTSP_SERVER)
    include_directories(devices/RtspServerDevice)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/RtspServerDevice/rtsp.hpp
        devices/RtspServerDevice/rtsp.cpp
        devices/RtspServerDevice/rtsp_factory.hpp
        devices/RtspServerDevice/rtsp_factory.cpp
        devices/RtspServerDevice/rtsp_settings.hpp
        )
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GST REQUIRED gstreamer-1.0>=1.4
        gstreamer-sdp-1.0>=1.4
        gstreamer-video-1.0>=1.4
        gstreamer-app-1.0>=1.4
        gstrtp-1.0
        glib-2.0
        )
    target_link_libraries(${CMAKE_PROJECT_NAME} ${GST_LIBRARIES} ${GST_LIBRARIES} gstapp-1.0)
    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${GST_INCLUDE_DIRS})
    if(${ARCH_ARM} EQUAL 1)
        add_subdirectory(${CMAKE_SOURCE_DIR}/devices/RtspServerDevice/rtsp_server) #RTSP сервер для Rockchip
    endif() # END if(${ARCH_ARM} EQUAL 1)
    if(${ARCH_ARM} EQUAL 0)
#        add_subdirectory(${CMAKE_SOURCE_DIR}/rtsp_server_X86) #RTSP сервер для Desktop пока не реализовано
    endif() # END if(${ARCH_ARM} EQUAL 0)
endif() # END if(${USE_RTSP_SERVER} EQUAL 1)

############################################ END DEVICES

if(${USE_ETHERNET_CONNECTION} EQUAL 1)
    add_definitions(-DUSE_ETHERNET_CONNECTION)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        eth/ethcontroller.cpp
        eth/ethcontroller.h
        eth/server.cpp
        eth/server.h
        eth/client.cpp
        eth/client.h
        eth/socket/tcpclient.cpp
        eth/socket/tcpsocket.h
        eth/socket/udpsocket.cpp
        eth/socket/socketaddress.cpp
        eth/socket/tcpclient.h
        eth/socket/udpclient.cpp
        eth/socket/udpsocket.h
        eth/socket/socketaddress.h
        eth/socket/tcpserver.cpp
        eth/socket/udpclient.h
        eth/socket/socket.cpp
        eth/socket/tcpserver.h
        eth/socket/udpserver.cpp
        eth/socket/socket.h
        eth/socket/tcpsocket.cpp
        eth/socket/udpserver.h
        cmd/cmdmanager.cpp
        cmd/cmdmanager.h
        )
    if(${USE_PCAP} EQUAL 1)
        add_definitions(-DPCAP)
        target_sources(${CMAKE_PROJECT_NAME} PRIVATE
            eth/socket/pcapclient.cpp
            eth/socket/pcapsocket.h
            eth/socket/pcapclient.h
            eth/socket/pcapserver.cpp
            eth/socket/pcapserver.h
            eth/socket/pcapsocket.cpp
            )
        target_link_libraries(${CMAKE_PROJECT_NAME} -lpcap)
    endif() # END if(${USE_PCAP} EQUAL 1)
endif() # END if(${USE_ETHERNET_CONNECTION} EQUAL 1)

if(${USE_CORSAIR_400_RAW} EQUAL 1)
    add_definitions(-USE_CORSAIR_400_RAW)
    if(${USE_DBG_SEND_VTRACK} EQUAL 1)
        add_definitions(-DUSE_DBG_SEND_VTRACK)
    endif() # END if(${USE_DBG_SEND_VTRACK} EQUAL 1)

    if(${DBG_VIDEO_SENDER} EQUAL 1)
        add_definitions(-DDBG_VIDEO_SENDER)
    endif() # END if(${DBG_VIDEO_SENDER} EQUAL 1)

    add_library(vtrack_data_sender SHARED
        modules/vtrack_data_sender/vtrack_data_struct.hpp
        modules/vtrack_data_sender/vtrack_data_sender.hpp
        modules/vtrack_data_sender/vtrack_data_sender.cpp
        modules/vtrack_data_sender/vtrack_data_sender_factory.hpp
        modules/vtrack_data_sender/vtrack_data_sender_factory.cpp
        )
    target_link_libraries(${CMAKE_PROJECT_NAME} vtrack_data_sender)

    add_subdirectory(modules/eth_controller_toolbox)
    include_directories(modules/eth_controller_toolbox/)

    add_library(cmd400_keeper SHARED
        modules/cmd400/cmd400_struct.hpp
        modules/cmd400/cmd400_keeper.hpp
        modules/cmd400/cmd400_keeper.cpp
        modules/cmd400/cmd400_keeper_factory.hpp
        modules/cmd400/cmd400_keeper_factory.cpp
        )
    target_link_libraries(${CMAKE_PROJECT_NAME} vtrack_data_sender cmd400_keeper eth_controller_toolbox_lib)
endif() # END if(${USE_CORSAIR_400_RAW} EQUAL 1)

target_link_libraries(${CMAKE_PROJECT_NAME} ${OpenCV_LIBS} ${GPU_LIB} ${trac_lib_name} -lrt -lpthread)
target_link_libraries(${CMAKE_PROJECT_NAME} ${OpenCV_LIBS} ${GPU_LIB} ${trac_lib_name} -lrt -lpthread)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${run_name})
