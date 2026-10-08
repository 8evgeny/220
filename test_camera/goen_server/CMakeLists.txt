cmake_minimum_required(VERSION 3.5)

# --- need install packets for work ---
# sudo apt install libserial-dev
# snap install mediamtx
# snap enable mediamtx
# change config file - /var/snap/mediamtx/common/mediamtx.yml
#  Перед первым запуском с USE_TV_cam=0 из папки tools/ необходимо запустить скрипт
# sudo ./ir_cam_setup.sh ${USER}

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")

project(goen_server)
message(STATUS "")
message(STATUS "-------------------------------- CMAKE BEGINING for project ${CMAKE_PROJECT_NAME}")

# Использование GPU:
set(USE_GPU 2) # (0 - CPU, 1 - CUDA, 2 - OPENCL)
# Тип корреляции при template matching для USE_CORR_TYPE:
# CORR_TYPE = 2 : SQDIFF_NORMED (Прямая корреляция)
# CORR_TYPE = 6 : CCOEFF_NORMED  (Кросскорреляция модифицированная)
# CORR_TYPE = 8 ; COMBINED (объединение прямой и модифицированной кросскорреляции)
# CORR_TYPE = 10 : TKDNN, RKNN
set(USE_CORR_TYPE 8)

if (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "x86_64")            #Десктоп
    message("SYSTEM_ARCH: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
    set(OpenCV_DIR $ENV{HOME}/opencv/install_4_10_0_contrib_cuda)
    add_definitions(-DARCH_X86_64)
    set(ARCH_ARM 0)
endif()
if (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "aarch64")           #Одноплатники ARM (Khadas, OrangePi5)
    message("SYSTEM_ARCH: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
    set(OpenCV_DIR $ENV{HOME}/opencv/install_4_7_0_contrib)
    add_definitions(-DARCH_ARM)
    set(ARCH_ARM 1)
endif()

set(USE_TV_cam              1)  # (TV_MIPI - 1 / TPV - 0)
set(USE_I2C                 1)  # (inklenometr ON - 1 / inklenometr OFF - 0 / inklenometr DMP - 2)
set(USE_GUI                 0)  # Работа в режиме GUI (медленно), или без GUI (быстро):
set(USE_LOGGER              1)  # Использование логгера
set(USE_MODE_STAB           0)  # 0 - stabilization on goen, 1 - stabilization on Khadas (or OLD firmware)
set(USE_CONFIRMATION        0)  # использовать передачу по ethernet подтверждения получения команды от клиента
set(USE_NUC_CONTROL         1)  # контроль затвора ТПВ камеры IVASTER
set(USE_CLOSE_RADIOLINE     1)  # 0 - OPEN RTSP видео, 1 - CLOSE, RTP видео + UDP телеметрия для автопилота
set(USE_DBG_PLOT            0)  # вывод абсолютных углов и скоростей на график
set(USE_TLM_MODE_CHANGER    1)  # возможность изменения способа передачи телеметрии в прямом эфире
set(USE_LOG_CMD             0)  # логирование команд, приходящих по RS232
set(USE_SERIAL_PTR          1)  # использование серийных портов (RS232/RS485)
# Use Devices:
set(USE_FOLDER_READER       1)  # 0
set(USE_WEB_CAMERA          1)  # 2

#######################################################

if(${USE_TV_cam} EQUAL 1)
    add_definitions(-DUSE_TV)
    set(USE_COLOR_CHANNEL_MODE 8UC3) # (8UC3 -- BGR, 8UC1 -- GRAY8, 8UC4 -- RGBA)
endif() # END if(${USE_TV_cam} EQUAL 1)
if(${USE_TV_cam} EQUAL 0)
    add_definitions(-DUSE_TPV_cam)
    set(USE_COLOR_CHANNEL_MODE 8UC1) # (8UC3 -- BGR, 8UC1 -- GRAY8, 8UC4 -- RGBA)
    if(${USE_NUC_CONTROL} EQUAL 1 AND ${USE_TV_cam} EQUAL 0)
        add_definitions(-DUSE_NUC_CONTROL)
    endif() # END  if(${USE_NUC_CONTROL} EQUAL 1)
endif() # if(${USE_TV_cam} EQUAL 0)


set(LIST_VALID_COLOR_CHANNEL_MODES 8UC1 8UC2 8UC3 8UC4)

if(NOT ${USE_COLOR_CHANNEL_MODE} IN_LIST LIST_VALID_COLOR_CHANNEL_MODES)
    message("Note: valid COLOR_CHANNEL_MODE list: " "${LIST_VALID_COLOR_CHANNEL_MODES}")
else()
    message(STATUS "USE_COLOR_CHANNEL_MODE = ${USE_COLOR_CHANNEL_MODE}")
    add_definitions(-DCCM_${USE_COLOR_CHANNEL_MODE})
endif()

if(${USE_GPU} EQUAL 0)
    set(GPU_PREFIX cpu)
    message(STATUS "--------------------------- USE_CPU")
endif() # END if(${USE_GPU} EQUAL 0)

if(${USE_GPU} EQUAL 2)
    include_directories($CMAKE_SOURCE_DIR})
    find_package(OpenCL REQUIRED)
    include_directories (${OpenCL_INCLUDE_DIRS})
    set(GPU_LIB ${OpenCL_LIBRARY})
    set(trac_lib_name -l_trac_tmml_${USE_CORR_TYPE}_CL) # Название подключаемой библиотеки OpenCL

    add_definitions(-DUSE_CL)
    message(STATUS "--------------------------- USE_CORR_TYPE=${USE_CORR_TYPE}")
    message(STATUS "--------------------------- USE_CL")
    add_definitions(-Dclass_name=trac_tmml)
    add_definitions(-Dcreate_name=create_track)
endif()

set(CMAKE_MAKE_PROGRAM "make -j${nproc}") # Число потоков компиляции.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/run) # директория сборки проекта

if(${USE_GUI} EQUAL 1)
    add_definitions(-DUSE_GUI)
    message(STATUS "--------------------------- USE_GUI=1")
else()
    set(SHOW_PSTFX )
    message(STATUS "--------------------------- USE_GUI=0")
endif() # if(${USE_GUI} EQUAL 1)

if(${USE_MODE_STAB} EQUAL 1)
    add_definitions(-DUSE_MODE_STAB)
    message(STATUS "--------------------------- USE_MODE_STAB=1")
elseif(${USE_MODE_STAB} EQUAL 0)
    set(SHOW_PSTFX )
    message(STATUS "--------------------------- USE_MODE_STAB=0")
endif() # elseif(${USE_MODE_STAB} EQUAL 0)

if(${USE_CONFIRMATION} EQUAL 1)
    add_definitions(-DUSE_CONFIRMATION)
endif() # if(${USE_CONFIRMATION} EQUAL 1)


if(${USE_CLOSE_RADIOLINE} EQUAL 1)
    add_definitions(-DUSE_RTP)
    add_definitions(-DUSE_UDP_TLM)
endif() # END if(${USE_CLOSE_RADIOLINE} EQUAL 3)


link_directories(${CMAKE_CURRENT_LIST_DIR})
find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)
message(OpenCV_INCLUDE_DIRS=${OpenCV_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${CMAKE_CURRENT_LIST_DIR}/lib)
include_directories(headers tools )
include_directories(tools/headers/)
include_directories(tools/mat2rtsp)
include_directories(LinuxI2CDev/I2Cdev)
include_directories(LinuxI2CDev/MPU6050)
include_directories(modules/RS232_worker)
include_directories(modules/RS485_worker)
include_directories(modules/CMD_over_UDP_receiver)
include_directories(modules/goen_60_struct)

file(GLOB TracShatsFiles "${CMAKE_CURRENT_LIST_DIR}/tracshats/*")
add_executable(${CMAKE_PROJECT_NAME}
    config.ini
    source/main.cpp
    source/application.cpp
    source/ports.cpp
    source/show_res.cpp
    source/from_goen_tlm_sender.cpp
    source/common_data.cpp
    source/nuc_control.cpp
    headers/nuc_control.hpp
    headers/application.hpp
    headers/common_data.hpp
    headers/kalman.hpp
    modules/goen_60_struct/rs485_struct.hpp
    modules/goen_60_struct/to_bort_struct.hpp
    modules/goen_60_struct/from_bort_struct.hpp
    modules/goen_60_struct/bool_uint_transformer.hpp
    modules/goen_60_struct/bool_uint_transformer.cpp
    modules/RS232_worker/RS232_worker.hpp
    modules/RS232_worker/RS232_worker.cpp
    modules/RS232_worker/cmd_from_rs232.cpp
    modules/RS232_worker/to_goen_struct.hpp
    modules/RS485_worker/RS485_worker.hpp
    modules/RS485_worker/RS485_worker.cpp
    modules/RS485_worker/cmd_to_rs485.cpp
    modules/CMD_over_UDP_receiver/CMD_over_UDP_receiver.hpp
    modules/CMD_over_UDP_receiver/CMD_over_UDP_receiver.cpp
    tools/mat2rtsp/mat2rtsp.hpp
    tools/mat2rtsp/mat2rtsp.cpp
    ##############  Trackers ###################################################
    ${TracShatsFiles}
    ##############  Tools  ####################################################
    tools/INIReader.h
    tools/watchdog.h
    tools/colors.h
    tools/toolbox_common_utils/toolbox_common_utils.cpp
    tools/toolbox_common_utils/toolbox_common_utils.hpp
    tools/time_keeper/time_keeper.cpp
    tools/time_keeper/time_keeper.hpp
    tools/temperature_monitor/temperature_monitor.cpp
    tools/temperature_monitor/temperature_monitor.hpp
    tools/custom_filesystem/custom_filesystem.hpp
    tools/custom_filesystem/custom_filesystem.cpp
    tools/
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

########################### BEGIN DEVICES ###########################
if(${USE_FOLDER_READER} EQUAL 1) # 0
    add_definitions(-DUSE_FOLDER_READER)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/FolderReader/folderreader.h
        devices/FolderReader/folderreader.cpp
        )
endif() # END if(${USE_FOLDER_READER} EQUAL 1)

if(${USE_WEB_CAMERA} EQUAL 1) # 2
    add_definitions(-DUSE_WEB_CAMERA)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/WebCamera/webcamera.hpp
        devices/WebCamera/webcamera.cpp
        )
endif() # END if(${USE_WEB_CAMERA} EQUAL 1)

if(${USE_GUI} EQUAL 0)
    set(GUI_RUN_PSTFX "notgui")
elseif(${USE_GUI} EQUAL 1)
    set(GUI_RUN_PSTFX "gui")
else()
    message(FATAL_ERROR "Error: incorrect \"USE_GUI\" parameter = \"#{USE_GUI}\"")
endif() # END if(NOT ${USE_GUI} EQUAL 0)


if(${USE_I2C} EQUAL 1)
    set(run_name run_${GUI_RUN_PSTFX}_${USE_COLOR_CHANNEL_MODE}_i2c)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        LinuxI2CDev/I2Cdev/I2Cdev.h
        LinuxI2CDev/I2Cdev/I2Cdev.cpp
        LinuxI2CDev/MPU6050/MPU6050.h
        LinuxI2CDev/MPU6050/MPU6050.cpp
        LinuxI2CDev/MPU6050/MPU6050_6Axis_MotionApps20.h
        LinuxI2CDev/MPU6050/MPU6050_6Axis_MotionApps20.cpp
        source/mpu6050useDMP.cpp
        headers/mpu6050useDMP.hpp)
    add_definitions(-DUSE_I2C_DMP)
    message(STATUS "--------------------------- USE_I2C_DMP")
endif() # END if(${USE_I2C} EQUAL 1)

if(${USE_DBG_PLOT} EQUAL 1)
    add_definitions(-DUSE_DBG_PLOT)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        tools/plot/plot.cpp
        tools/plot/plot.hpp
        tools/graph/graph.cpp
        tools/graph/graph.hpp
)
endif() # END if(${USE_DBG_PLOT} EQUAL 0)

if(${USE_TLM_MODE_CHANGER} EQUAL 1)
    add_definitions(-DUSE_TLM_MODE_CHANGER)
endif() # END if(${USE_TLM_MODE_CHANGER} EQUAL 1)

if(${USE_LOG_CMD} EQUAL 1)
    add_definitions(-DUSE_LOG_CMD)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE tools/log_cmd/log_cmd.hpp)
endif() # END if(${USE_LOG_CMD} EQUAL 1)

if(${USE_SERIAL_PTR} EQUAL 1)
    add_definitions(-DUSE_SERIAL_PTR)
endif()

if(${USE_I2C} EQUAL 0)
    set(run_name run_${GUI_RUN_PSTFX}_${USE_COLOR_CHANNEL_MODE})
    message(STATUS "--------------------------- NO USE_I2C")
endif() # END if(${USE_I2C} EQUAL 0)

target_link_libraries(${CMAKE_PROJECT_NAME}
    serial
    ${OpenCV_LIBS}
    ${GPU_LIB}
    ${trac_lib_name}
    -lrt
    -lpthread
)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${run_name})
