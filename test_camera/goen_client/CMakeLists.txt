cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

# --- need install packets for work ---
# При первой установке программы на компьютер запустите скрипт  desctop/add_multicast.sh и следуйте инструкции
# sudo apt install libserial-dev
# если не запускается поток rtp по порту 8001, использовать команду "sudo fuser -k 8001/udp" до тех пор, пока вывод терминала в ответ на команду не будет пустым (на этом порте, обязательном для закрытой линии, могут быть неважные для работы клиента процессы snap)

project(goen_client)
message(STATUS "-------------------------------- CMAKE BEGINING for project ${CMAKE_PROJECT_NAME}")

set(OpenCV_DIR $ENV{HOME}/opencv/install_4_7_0_contrib_cuda)

#################################################################################################

# Использование логгера
set(USE_LOGGER 1)

#Выбор канала передачи команд
set(USE_UDP 1)
set(USE_RS232 0)

set(USE_LOG_CMD 1)
set(USE_CONFIRMATION 0)

# Use Devices:
set(USE_FOLDER_READER 1)
set(USE_GST_PIPELINE_DEVICE 1)

#################################################################################################

set(CMAKE_MAKE_PROGRAM "make -j${nproc}") # Число потоков компиляции.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/run) # директория сборки проекта

link_directories(${CMAKE_CURRENT_LIST_DIR})

find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)
message(OpenCV_INCLUDE_DIRS=${OpenCV_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(tools/)

include_directories(modules/cv_widgets/)
file(GLOB cv_widgets "${CMAKE_CURRENT_LIST_DIR}/modules/cv_widgets/*/*" "${CMAKE_CURRENT_LIST_DIR}/modules/cv_widgets/*")

include_directories(modules/goen_60_struct/)
file(GLOB goen_60_struct "${CMAKE_CURRENT_LIST_DIR}/modules/goen_60_struct/*/*" "${CMAKE_CURRENT_LIST_DIR}/modules/goen_60_struct/*")

#include_directories(modules/xserial/)
#file(GLOB xserial "${CMAKE_CURRENT_LIST_DIR}/modules/xserial/*/*" "${CMAKE_CURRENT_LIST_DIR}/modules/xserial/*")

#include_directories(modules/RS232_transiever/)
#file(GLOB RS232_transiever "${CMAKE_CURRENT_LIST_DIR}/modules/RS232_transiever/*/*" "${CMAKE_CURRENT_LIST_DIR}/modules/RS232_transiever/*")

include_directories(modules/RS232_transieverLS/)
file(GLOB RS232_transieverLS "${CMAKE_CURRENT_LIST_DIR}/modules/RS232_transieverLS/*/*" "${CMAKE_CURRENT_LIST_DIR}/modules/RS232_transieverLS/*")

include_directories(modules/ethernet_lib/)
file(GLOB UDP_lib "${CMAKE_CURRENT_LIST_DIR}/modules/ethernet_lib/*")

add_executable(${CMAKE_PROJECT_NAME}
    config.ini
    main.cpp
    main_window_constructor.hpp
    main_window_constructor.cpp
    commands.hpp
    ${cv_widgets}
    ${goen_60_struct}
    ${RS232_transieverLS}
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

set(RUN_NAME "")
if(${USE_UDP} EQUAL 1)
    add_definitions(-DUSE_UDP)
    message(STATUS "SEND COMMANDS VIA USE_UDP=1")
    string(APPEND RUN_NAME "_UDP")
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE  ${UDP_lib})
endif()
if(${USE_RS232} EQUAL 1)
    add_definitions(-DUSE_RS232)
    message(STATUS "SEND COMMANDS VIA USE_RS232=1")
    string(APPEND RUN_NAME "_RS232")
endif()

if(${USE_LOG_CMD} EQUAL 1)
    add_definitions(-DUSE_LOG_CMD)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        tools/log_cmd/log_cmd.hpp)
endif() # if(${USE_LOG_CMD} EQUAL 1)

if(${USE_LOGGER} EQUAL 1)
    add_definitions(-DUSE_LOGGER)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        logger/logger.cpp
        logger/logger.hpp
        logger/factory.cpp
        logger/factory.h
        logger/settings.hpp)
endif() # END if(${USE_LOGGER} EQUAL 1)

if(${USE_CONFIRMATION} EQUAL 1)
    add_definitions(-DUSE_CONFIRMATION)
endif()

########################### BEGIN DEVICES:

if(${USE_FOLDER_READER} EQUAL 1) # 0
    add_definitions(-DUSE_FOLDER_READER)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/FolderReader/folderreader.h
        devices/FolderReader/folderreader.cpp
        )
endif() # END if(${USE_FOLDER_READER} EQUAL 1)


if(${USE_GST_PIPELINE_DEVICE} EQUAL 1) # 7
    add_definitions(-DUSE_GST_PIPELINE_DEVICE)
    include_directories(tools/)
    include_directories(tools/toolbox_common_utils/)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        devices/GstPipelineDevice/gst_pipeline_device.hpp
        devices/GstPipelineDevice/gst_pipeline_device.cpp
        devices/GstPipelineDevice/gst_pipeline_device_factory.hpp
        devices/GstPipelineDevice/gst_pipeline_device_factory.cpp
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
endif() # END if(${USE_GST_PIPELINE_DEVICE} EQUAL 1)

# sudo apt-get install libjsoncpp-dev
# pkg_search_module(JSONCPP jsoncpp)
# target_include_directories(${PROJECT_NAME} PRIVATE ${JsonCpp_INCLUDE_DIRS})
# ${JSONCPP_LIBRARIES}


############################################ END DEVICES

set(run_name ${CMAKE_PROJECT_NAME}${RUN_NAME})

target_link_libraries(${CMAKE_PROJECT_NAME} ${OpenCV_LIBS} -lrt -lpthread -lserial )
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${run_name})
