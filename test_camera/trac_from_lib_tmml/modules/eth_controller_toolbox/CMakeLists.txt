# project(testlib1)

# set(USE_ETH_DEBUG_LOG_TERMINAL_MODE 0) # TODO расширенное терминальное логирование работы программы
set(COMPILE_WITH_OPENCV 1) # для отправки видео нужна либа OpenCV, для командного обмена - нет

# CLIENT / SERVER controllers/modules for commands 
set(USE_ASYNC_TCP_ETH_CMD_CONTROLLER 0) # асинхронный TCP Ethernet контроллер приема/отправки команд
set(USE_ASYNC_UDP_ETH_CMD_CONTROLLER 1) # асинхронный UDP Ethernet контроллер приема/отправки команд
set(USE_ASYNC_UDP_CLIENT 1)
set(USE_ASYNC_UDP_SERVER 1)

set(USE_COMPILE_ALL_CMD 0) # "1" - собираются все найденные в директории commands_zoO
set(USE_GST_DEMO 0) # gstreamer send receive video (отдельный CMakeLists)
set(USE_VIDEO_DEMO 0) 

# Базовые утилиты toolbox'а 
set(USE_BASE_CMD_UDP_SERVER 1) # базовый сервер команд (базовый = без обвязки в виде cmdManager, observer-паттерна и т.д.)
set(USE_BASE_CMD_UDP_CLIENT 1) # базовый клиент команд

### VIDEO ###
set(USE_GSTCV_MJPG_ETH_CONTROLLER           0)
set(USE_UDP_ETH_GSTCV_RAW_VIDEO_RECEIVER    0) # в разработке
set(USE_UDP_ETH_GSTCV_RAW_VIDEO_TRANSMITTER 0) # в разработке
set(USE_UDP_RAW_GRAY_VIDEO_SERVER           0) 
set(USE_UDP_RAW_GRAY_VIDEO_CLIENT           0)
set(USE_GSTCV_SRT_CLIENT                    0)
set(USE_GSTCV_SRT_SERVER                    0)
set(USE_GSTC_RTPX264_CLIENT                 0) # gstreamer c-api
set(USE_GSTC_RTPX264_SERVER                 1)

### COMMANDS ###
# Выбор определенных eth команд для компиляции
set(USE_ETH_CMD_UNCOMPRESS_FRAME_DEFAULT 0)   # Отправка RAW-изображений (deprecated) как команд
set(USE_ETH_CMD_COPTER_ALARM 0)               # Команда ATAS 
set(USE_ETH_CMD_TELEMETRY_ATAS 0)             # Телеметрия ATAS
set(USE_ETH_CMD_POWEROFF_REQUEST 0)           # Команда-запрос на выключение абонента 
set(USE_ETH_CMD_CONFIRM_CMD_RECEIVE 0)        # Команда-подтверждение получения некоторого сообщения по определенному ключу
set(USE_ETH_CMD_AP_TELEMETRY 0)               # Телеметрия автопилота UAV
set(USE_ETH_CMD_TRACKER_METADATA 0)           # Метаданные навигационного теркинга UAV
set(USE_ETH_CMD_TELEMETRY_ORIENTATION 0)      # Телеметрия ориентации по крену, тангажу и рысканью
set(USE_ETH_CMD_AP_POS_CNT 0)                 # Контроль положения БПЛА для отправки на автопилот (АП)
set(USE_ETH_CMD_FENIX2META 0)                 # Телеметрия fenix2
set(USE_ETH_CMD_FENIX2CMD 0)                  # Команды fenix2 (клиент-сервер)
set(USE_ETH_CMD_400_INIT 1)                   # Команда инициализации трекера для шара
set(USE_ETH_CMD_RECORD_ON 1)                  # команда удалённого включения/выключения записи
set(USE_ETH_CMD_400_DEINIT 1)                 # Команды деинициализации трекера для шара
set(USE_ETH_CMD_VERTELKA_CLOSE 1)             # Команды удаленного запуска алгоритма трекера на шаре
set(USE_ETH_CMD_VERTELKA_RUN10 1)             # Команды удаленного запуска алгоритма трекера на шаре
set(USE_ETH_CMD_VERTELKA_RUN8 1)              # Команды удаленного запуска алгоритма трекера на шаре

### TESTS ###
set(COMPILE_ALL_TESTS 0) # 0 - NOT, 1 - YES; Скорее всего потребуется USE_COMPILE_ALL_CMD = 1 !
set(USE_TEST_ASYNC_UDP_CLIENT 0)
set(USE_TEST_ASYNC_UDP_SERVER 0)
set(USE_TEST_MANUAL_ATAS_TELEMETRY 0)
set(USE_TEST_MANUAL_CONFIRMATION_CMD_RECEIVE 0)
set(USE_TEST_SETIALIZATION_STRING 0)
set(USE_TEST_TABLE_ID_REPLACE 0)
set(USE_TEST_RCV_ORIENT_TELE 0)
set(USE_TEST_RCV_AP_POS_CNT 0)
set(USE_TEST_SND_AP_POS_CNT 0)
set(USE_TEST_BASIC_UDP_SERVER 0)
set(USE_TEST_BASIC_UDP_CLIENT 0)
set(USE_TEST_CHRONO_NEGATIVE_TIME 0)
set(USE_TEST_GSTC_RTPX264_SERVER 0)
set(USE_TEST_GSTC_RTPX264_CLIENT 0)



### Приложения ### 
set(COMPILE_ALL_APPS 0) # приложения в рамках Eth_Controller (например, сервер-ретранслятор телеметрии)

#[[
  Приложение принимает телеметрию в формате orientation, 
  преобразовывает в ap_telemetry 
  и отправляет получателю ap_tele_keeper
]]#
set(USE_APP_RETRANSLATOR_TELE_ORIENT_TO_AP 0) 

# DEPRECATED BLOCK
set(USE_TCP_ETH_CONTROLLERS_FAMILY 0) # !! deprecated семейство синхронных контроллеров приема/отправки команд и изображений (deprecated)


###############################################################################

if(${COMPILE_ALL_TESTS} EQUAL 1 OR ${COMPILE_ALL_APPS} EQUAL 1)
  set(USE_ASYNC_UDP_ETH_CMD_CONTROLLER 1)
  set(USE_COMPILE_ALL_CMD 1)
endif()

if(${USE_APP_RETRANSLATOR_TELE_ORIENT_TO_AP} EQUAL 1)
  set(USE_ETH_CMD_AP_TELEMETRY 1)
  set(USE_ETH_CMD_TELEMETRY_ORIENTATION 1)
  set(USE_ASYNC_UDP_ETH_CMD_CONTROLLER 1)
endif()

if(${USE_COMPILE_ALL_CMD} EQUAL 1)
  set(COMPILE_WITH_OPENCV 1)
endif()

if(
  (${USE_GSTCV_MJPG_ETH_CONTROLLER}             EQUAL 1)  OR
  (${USE_UDP_ETH_GSTCV_RAW_VIDEO_RECEIVER}      EQUAL 1)  OR
  (${USE_UDP_ETH_GSTCV_RAW_VIDEO_TRANSMITTER}   EQUAL 1)  OR
  (${USE_UDP_RAW_GRAY_VIDEO_SERVER}             EQUAL 1)  OR
  (${USE_UDP_RAW_GRAY_VIDEO_CLIENT}             EQUAL 1)  OR
  (${USE_GSTCV_SRT_CLIENT}                      EQUAL 1)  OR
  (${USE_GSTCV_SRT_SERVER}                      EQUAL 1)  OR
  (${USE_GSTC_RTPX264_CLIENT}                   EQUAL 1)  OR
  (${USE_GSTC_RTPX264_SERVER}                   EQUAL 1)
  ) 
  set(COMPILE_WITH_OPENCV 1)
endif()

if(${USE_GST_DEMO} EQUAL 1)
  add_subdirectory(demo/)
endif()

add_definitions(-DSO_REUSEPORT)

# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "/")
# message("PROJECT_BINARY_DIR=" ${PROJECT_BINARY_DIR}) # PROJECT_BINARY_DIR CMAKE_BINARY_DIR
# message("CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR})
# message("CMAKE_CURRENT_LIST_DIR=" ${CMAKE_CURRENT_LIST_DIR})
# message("CMAKE_CURRENT_BINARY_DIR=" ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib/")

# Макрос предоставляет возможность получить список субдиректорий некоторого каталога
# Интерфейс использования: SUBDIRLIST(SUBDIRS ${MY_CURRENT_DIR})
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

# макрос принимает путь к директории, сканирует директорию, определяет субдиректории (на 1 уровень в глубину), добавляет в общ. список пути к cpp и hpp файлам
MACRO(GET_ALL_TARGETS_FAMILY targets_list path2family_dir)
    SET(gen_files_list "")
    SUBDIRLIST(SUBDIRS_TMP ${path2family_dir})
    foreach(family_item ${SUBDIRS_TMP})
        file(GLOB part_file_list "${path2family_dir}/${family_item}/*.hpp" "${path2family_dir}/${family_item}/*.cpp")
        list(APPEND gen_files_list ${part_file_list})
    endforeach()
    set(${targets_list} ${gen_files_list})
ENDMACRO()

#[[
  Макрос извлекает список полных путей к файлам для сблоки.
  @param targets_list_dst - возвращаемый список полных путей к файлам
  @param path2family_dir_src - путь к директории, в которой находятся субдиректории
  @param selected_subdir_names_src - названия субдиректорий, в которой определяются файлы,
    полные пути к которым добавляются в targets_list_dst
]]#
MACRO(GET_SELECTED_TARGETS_FAMILY targets_list_dst path2family_dir_src selected_subdir_names_src)
  SET(gen_files_list "")
  foreach(family_item ${selected_subdir_names_src})
    file(GLOB part_file_list 
      "${path2family_dir_src}/${family_item}/*.hpp" 
      "${path2family_dir_src}/${family_item}/*.cpp")
    list(APPEND gen_files_list ${part_file_list})
  endforeach()
  set(${targets_list_dst} ${gen_files_list})
ENDMACRO()

if(${COMPILE_WITH_OPENCV} EQUAL 1)
  set(OpenCV_DIR $ENV{HOME}/opencv/install_4_7_0_contrib_cuda)
  find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)
endif()


# TODO перенести в отдельные блоки, чтобы не засорять include-пространство, если они отключены

set(ETH_CONTROLLER_INCLUDE_DIRS
  "${CMAKE_CURRENT_LIST_DIR}/udp/"

  PARENT_SCOPE
  )

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/
#   ${CMAKE_CURRENT_LIST_DIR}/udp/
#   ${CMAKE_CURRENT_LIST_DIR}/eth_cnt_common_utils/
#   ${CMAKE_CURRENT_LIST_DIR}/toolbox_common_utils/
#   ${CMAKE_CURRENT_LIST_DIR}/notification_handler/
#   ${CMAKE_CURRENT_LIST_DIR}/video_eth_controllers/
#   ${CMAKE_CURRENT_LIST_DIR}/command_processing/
#   ${CMAKE_CURRENT_LIST_DIR}/command_processing/commands_zoo/
#   ${CMAKE_CURRENT_LIST_DIR}/command_processing/cmd_manager/
  )

add_library(eth_controller_toolbox_lib SHARED

    # common types/utils
    eth_cntrl_common_types.hpp
    eth_cnt_common_utils/eth_cnt_common_utils.hpp
    eth_cnt_common_utils/eth_cnt_common_utils.cpp

    # toolbox_common_utils
    toolbox_common_utils/toolbox_common_utils.hpp
    toolbox_common_utils/toolbox_common_utils.cpp

    # inotifyhandler
    notification_handler/iethnotifyhandler.cpp
    notification_handler/iethnotifyhandler.hpp
    notification_handler/eth_notification_common.hpp

    # sys time extract
    tools/sys_time_extract/sys_time_extract.hpp
    tools/sys_time_extract/sys_time_extract.cpp

    # eth_log_tools
    tools/eth_log_tools/eth_log_tools.hpp
    tools/eth_log_tools/eth_log_tools.cpp
    )

if(${COMPILE_WITH_OPENCV} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
    # frame handler (direct)
    i_direct_frame_handler.hpp
    i_direct_frame_handler.cpp
  )
endif()



if(${USE_ASYNC_UDP_ETH_CMD_CONTROLLER} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
      # async_udp_eth_controller_cmd -- контроллер асинхронного взаимодействия на отправку и прием cmd через eth
      udp_modules/async_udp_eth_controller_cmd/async_udp_eth_controller_cmd.hpp
      udp_modules/async_udp_eth_controller_cmd/async_udp_eth_controller_cmd.cpp
      udp_modules/async_udp_eth_controller_cmd/async_udp_eth_controller_cmd_factory.hpp
      udp_modules/async_udp_eth_controller_cmd/async_udp_eth_controller_cmd_factory.cpp
  
      udp_modules/async_udp_eth_controller_cmd/async_udp_client_simplex_cmd/async_udp_client_simplex_cmd.hpp
      udp_modules/async_udp_eth_controller_cmd/async_udp_client_simplex_cmd/async_udp_client_simplex_cmd.cpp
  
      udp_modules/async_udp_eth_controller_cmd/async_udp_server_simplex_cmd/async_udp_server_simplex_cmd.hpp
      udp_modules/async_udp_eth_controller_cmd/async_udp_server_simplex_cmd/async_udp_server_simplex_cmd.cpp
    )
  set(COMPILE_WITH_OPENCV 1)  
endif()

if(${USE_ASYNC_TCP_ETH_CMD_CONTROLLER} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
    async_tcp_eth_controller_cmd/async_tcp_eth_controller_cmd.hpp
    async_tcp_eth_controller_cmd/async_tcp_eth_controller_cmd.cpp
    async_tcp_eth_controller_cmd/async_tcp_eth_controller_cmd_factory.hpp
    async_tcp_eth_controller_cmd/async_tcp_eth_controller_cmd_factory.cpp

    async_tcp_eth_controller_cmd/async_tcp_client_simplex_cmd/async_tcp_client_simplex_cmd.hpp
    async_tcp_eth_controller_cmd/async_tcp_client_simplex_cmd/async_tcp_client_simplex_cmd.cpp

    async_tcp_eth_controller_cmd/async_tcp_server_simplex_cmd/async_tcp_server_simplex_cmd.hpp
    async_tcp_eth_controller_cmd/async_tcp_server_simplex_cmd/async_tcp_server_simplex_cmd.cpp
  )
endif()

if(${USE_ASYNC_UDP_CLIENT} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
    udp_modules/async_udp_client/async_udp_client.hpp
    udp_modules/async_udp_client/async_udp_client.cpp
  )
endif()

if(${USE_ASYNC_UDP_SERVER} EQUAL 1)
target_sources(eth_controller_toolbox_lib PRIVATE
    udp_modules/async_udp_server/async_udp_server.hpp
    udp_modules/async_udp_server/async_udp_server.cpp
  )  
endif()

if(${USE_GSTCV_MJPG_ETH_CONTROLLER} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
  video_eth_controllers/gstcv_mjpg_eth_controller/gstcv_mjpg_eth_controller.hpp
  video_eth_controllers/gstcv_mjpg_eth_controller/gstcv_mjpg_eth_controller.cpp
  video_eth_controllers/gstcv_mjpg_eth_controller/gstcv_mjpg_eth_controller_factory.hpp
  video_eth_controllers/gstcv_mjpg_eth_controller/gstcv_mjpg_eth_controller_factory.cpp)
endif()

if(${USE_UDP_ETH_GSTCV_RAW_VIDEO_RECEIVER} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
    video_eth_controllers/udp_eth_gstcv_raw_video/udp_eth_gstcv_raw_video_receiver/udp_eth_gstcv_raw_video_receiver.cpp
    video_eth_controllers/udp_eth_gstcv_raw_video/udp_eth_gstcv_raw_video_receiver/udp_eth_gstcv_raw_video_receiver.hpp
    video_eth_controllers/udp_eth_gstcv_raw_video/udp_eth_gstcv_raw_video_receiver/udp_eth_gstcv_raw_video_receiver_factory.hpp
    video_eth_controllers/udp_eth_gstcv_raw_video/udp_eth_gstcv_raw_video_receiver/udp_eth_gstcv_raw_video_receiver_factory.cpp
  )
endif()

if(${USE_UDP_ETH_GSTCV_RAW_VIDEO_TRANSMITTER} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
    video_eth_controllers/udp_eth_gstcv_raw_video/udp_eth_gstcv_raw_video_transmitter/udp_eth_gstcv_raw_video_transmitter.cpp
    video_eth_controllers/udp_eth_gstcv_raw_video/udp_eth_gstcv_raw_video_transmitter/udp_eth_gstcv_raw_video_transmitter.hpp
    video_eth_controllers/udp_eth_gstcv_raw_video/udp_eth_gstcv_raw_video_transmitter/udp_eth_gstcv_raw_video_transmitter_factory.cpp
    video_eth_controllers/udp_eth_gstcv_raw_video/udp_eth_gstcv_raw_video_transmitter/udp_eth_gstcv_raw_video_transmitter_factory.hpp
  )
endif()

if(${USE_UDP_RAW_GRAY_VIDEO_SERVER} EQUAL 1 OR
${USE_UDP_RAW_GRAY_VIDEO_CLIENT} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
  
  video_eth_controllers/tools/ts_pipe_frame_buffer/ts_pipe_frame_buf.hpp
  video_eth_controllers/tools/ts_pipe_frame_buffer/ts_pipe_frame_buf.cpp
  video_eth_controllers/tools/ts_pipe_frame_buffer/async_frame_bank.hpp
  video_eth_controllers/tools/ts_pipe_frame_buffer/async_frame_bank.cpp
  )
endif()

if(${USE_UDP_RAW_GRAY_VIDEO_SERVER} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
    video_eth_controllers/udp_eth_raw_gray_video/udp_raw_gray_video_server/udp_raw_gray_video_server.hpp
    video_eth_controllers/udp_eth_raw_gray_video/udp_raw_gray_video_server/udp_raw_gray_video_server.cpp
  )
endif()

if(${USE_UDP_RAW_GRAY_VIDEO_CLIENT} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
    video_eth_controllers/udp_eth_raw_gray_video/udp_raw_gray_video_client/udp_raw_gray_video_client.hpp
    video_eth_controllers/udp_eth_raw_gray_video/udp_raw_gray_video_client/udp_raw_gray_video_client.cpp
  )
endif()

if(${USE_GSTCV_SRT_CLIENT} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
    video_eth_controllers/srt/gstcv_srt_client/gstcv_srt_client.hpp
    video_eth_controllers/srt/gstcv_srt_client/gstcv_srt_client.cpp
  )
endif()

if(${USE_GSTCV_SRT_SERVER} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
    video_eth_controllers/srt/gstcv_srt_server/gstcv_srt_server.hpp
    video_eth_controllers/srt/gstcv_srt_server/gstcv_srt_server.cpp
  )
endif()

if(${USE_GSTC_RTPX264_CLIENT} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
    video_eth_controllers/gstc/rtp_client_x264/rtp_client.cpp
    video_eth_controllers/gstc/rtp_client_x264/rtp_client.hpp
video_eth_controllers/gstc/rtp_client_x264/rtp_client_settings.cpp
    video_eth_controllers/gstc/rtp_client_x264/rtp_client_settings.hpp
../fenix2meta/fenix2meta_struct.hpp
  )
endif()

if(${USE_GSTC_RTPX264_SERVER} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
    video_eth_controllers/gstc/rtp_server_x264/rtp_server.cpp
    video_eth_controllers/gstc/rtp_server_x264/rtp_server.hpp
video_eth_controllers/gstc/rtp_server_x264/rtp_server_settings.cpp
    video_eth_controllers/gstc/rtp_server_x264/rtp_server_settings.hpp
../fenix2meta/fenix2meta_struct.hpp
  )
endif()

if(${USE_GSTC_RTPX264_CLIENT} EQUAL 1 OR ${USE_GSTC_RTPX264_SERVER} EQUAL 1)
  find_package(PkgConfig REQUIRED) 
  pkg_search_module(GST REQUIRED gstreamer-1.0>=1.4
      gstreamer-sdp-1.0>=1.4
      gstreamer-video-1.0>=1.4
      gstreamer-app-1.0>=1.4
  gstreamer-rtp-1.0>=1.4
      gstreamer-udp-1.0>=1.4
      gstreamer-h264-1.0>=1.4
      gstreamer-1.0
      gstreamer-webrtc-1.0
      gstreamer-pbutils-1.0
      gstreamer-check-1.0
      gstrtp-1.0
      glib-2.0
      gobject-2.0
  )

pkg_search_module(GLIB REQUIRED IMPORTED_TARGET glib-2.0)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(gstreamer_sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0>=1.4)
pkg_search_module(gstreamer_app REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.4)
pkg_search_module(gstreamer_video REQUIRED IMPORTED_TARGET gstreamer-video-1.0>=1.4)
pkg_search_module(gstreamer_rtp REQUIRED IMPORTED_TARGET gstreamer-rtp-1.0>=1.4)


  include_directories(${GST_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(eth_controller_toolbox_lib ${GST_LIBRARIES}
      PkgConfig::gstreamer
      PkgConfig::gstreamer_sdp
      PkgConfig::gstreamer_app
      PkgConfig::gstreamer_video
      ${GLIB_LIBRARIES}
      ${gstreamer_LIBRARIES}
      ${gstreamer_sdp_LIBRARIES}
      ${gstreamer_app_LIBRARIES}
      ${gstreamer_video_LIBRARIES}
      ${gstreamer_rtp_LIBRARIES}
      gstapp-1.0
      ${OpenCV_LIBS}
      pthread)   
endif()

# deprecated
if(${USE_TCP_ETH_CONTROLLERS_FAMILY} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
      # client simplex
      tcp_client_simplex_zoo/tcp_client_simplex_base.hpp
      tcp_client_simplex_zoo/tcp_client_simplex_base.cpp
  
      # server simplex
      tcp_server_simplex_zoo/tcp_server_simplex_base.hpp
      tcp_server_simplex_zoo/tcp_server_simplex_base.cpp
  
      # tcp_eth_controllers_zoo
      tcp_eth_controllers_zoo/tcp_eth_controller_base.hpp
      tcp_eth_controllers_zoo/tcp_eth_controller_base.cpp
  )

  # target sources tcp_client_simplex family
  set(tcp_client_simplex_zoo_path "${CMAKE_CURRENT_LIST_DIR}/tcp_client_simplex_zoo/")
  GET_ALL_TARGETS_FAMILY(tcp_client_simp_family_files ${tcp_client_simplex_zoo_path})
  target_sources(eth_controller_toolbox_lib PRIVATE ${tcp_client_simp_family_files})


  # target sources tcp_server_simplex_family
  set(tcp_server_simplex_zoo_path "${CMAKE_CURRENT_LIST_DIR}/tcp_server_simplex_zoo/")
  GET_ALL_TARGETS_FAMILY(tcp_server_simp_family_files ${tcp_server_simplex_zoo_path})
  target_sources(eth_controller_toolbox_lib PRIVATE ${tcp_server_simp_family_files})

  # target sources eth_controller's family
  set(eth_controllers_zoo_path "${CMAKE_CURRENT_LIST_DIR}/tcp_eth_controllers_zoo/")
  GET_ALL_TARGETS_FAMILY(eth_cnt_family_files ${eth_controllers_zoo_path})
  target_sources(eth_controller_toolbox_lib PRIVATE ${eth_cnt_family_files})

  set(COMPILE_WITH_OPENCV 1)
endif()



target_sources(eth_controller_toolbox_lib PRIVATE
  command_processing/commands_zoo/eth_cmd_base.hpp
  command_processing/commands_zoo/eth_cmd_base.cpp
  command_processing/commands_zoo/eth_cmd_factory.hpp
  command_processing/commands_zoo/eth_cmd_factory.cpp
  command_processing/eth_cmd_classification.hpp
  command_processing/eth_cmd_classification.cpp
  command_processing/eth_serialization_tools.hpp
  command_processing/eth_serialization_tools.cpp
  command_processing/eth_cmd_common_types.hpp
  command_processing/icmdhandler.hpp
  command_processing/icmdhandler.cpp
  command_processing/table_cmd_id_replace/table_cmd_id_replace.hpp
  command_processing/table_cmd_id_replace/table_cmd_id_replace.cpp
  command_processing/cmd_handlers_distributor/cmd_handlers_distributor.hpp
  command_processing/cmd_handlers_distributor/cmd_handlers_distributor.cpp
  command_processing/cmd_manager/cmd_manager.hpp
  command_processing/cmd_manager/cmd_manager.cpp

    ### ID PREAMBULE ###
  command_processing/cmd_preambule_tools/cmd_id/cmd_id_codec/eth_cmd_id_codec.hpp
  command_processing/cmd_preambule_tools/cmd_id/cmd_id_codec/eth_cmd_id_codec.cpp
  command_processing/cmd_preambule_tools/cmd_id/cmd_id_subcodec_zoo/cmd_id_subcodec_base.hpp
  command_processing/cmd_preambule_tools/cmd_id/cmd_id_subcodec_zoo/id_pass/eth_cmd_id_pass.hpp
  command_processing/cmd_preambule_tools/cmd_id/cmd_id_subcodec_zoo/id_pass/eth_cmd_id_pass.cpp
  command_processing/cmd_preambule_tools/cmd_id/cmd_id_subcodec_zoo/id_simple_direct_enc/eth_cmd_id_simple_direct_enc.hpp
  command_processing/cmd_preambule_tools/cmd_id/cmd_id_subcodec_zoo/id_simple_direct_enc/eth_cmd_id_simple_direct_enc.cpp
  command_processing/cmd_preambule_tools/cmd_id/cmd_id_common.hpp

    ### CHECKSUM ###
  command_processing/cmd_tail_tools/checksum/checksum_codec/checksum_codec.hpp
  command_processing/cmd_tail_tools/checksum/checksum_codec/checksum_codec.cpp
  command_processing/cmd_tail_tools/checksum/checksum_zoo/checksum_base.hpp
  command_processing/cmd_tail_tools/checksum/checksum_zoo/checksum_xor/checksum_xor.hpp
  command_processing/cmd_tail_tools/checksum/checksum_zoo/checksum_xor/checksum_xor.cpp
  command_processing/cmd_tail_tools/checksum/checksum_zoo/checksum_pass/checksum_pass.hpp
  command_processing/cmd_tail_tools/checksum/checksum_zoo/checksum_pass/checksum_pass.cpp
  command_processing/cmd_tail_tools/checksum/checksum_tools/checksum_tools.hpp
  command_processing/cmd_tail_tools/checksum/checksum_tools/checksum_tools.cpp
  command_processing/cmd_tail_tools/checksum/checksum_common.hpp
  )
  


# basic toolbox
if(${USE_BASE_CMD_UDP_SERVER} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
    basic_toolbox/cmd_udp_server/cmd_udp_server.hpp
    basic_toolbox/cmd_udp_server/cmd_udp_server.cpp)
endif()

if(${USE_BASE_CMD_UDP_CLIENT} EQUAL 1)
  target_sources(eth_controller_toolbox_lib PRIVATE
    basic_toolbox/cmd_udp_client/cmd_udp_client.hpp
    basic_toolbox/cmd_udp_client/cmd_udp_client.cpp)
endif()


# Сборка команд

set(cmds_flag_aliases
"USE_ETH_CMD_UNCOMPRESS_FRAME_DEFAULT"
"USE_ETH_CMD_COPTER_ALARM"
"USE_ETH_CMD_TELEMETRY_ATAS"
"USE_ETH_CMD_POWEROFF_REQUEST"
"USE_ETH_CMD_CONFIRM_CMD_RECEIVE"
"USE_ETH_CMD_AP_TELEMETRY"
"USE_ETH_CMD_TRACKER_METADATA"
"USE_ETH_CMD_TELEMETRY_ORIENTATION"
"USE_ETH_CMD_AP_POS_CNT"
"USE_ETH_CMD_FENIX2META"
"USE_ETH_CMD_FENIX2CMD"
"USE_ETH_CMD_400_INIT"
"USE_ETH_CMD_400_DEINIT"
"USE_ETH_CMD_RECORD_ON"
"USE_ETH_CMD_VERTELKA_RUN10"
"USE_ETH_CMD_VERTELKA_RUN8"
"USE_ETH_CMD_VERTELKA_CLOSE"
)

foreach(cmd_flag ${cmds_flag_aliases})
if(${cmd_flag} EQUAL 1)
  message(" ----- ADD CMD ${cmd_flag} TO COMPILE")
  add_definitions("-D${cmd_flag}")
endif()
endforeach()

set(cmd_zoo_path "${CMAKE_CURRENT_LIST_DIR}/command_processing/commands_zoo/")
if(${USE_COMPILE_ALL_CMD} EQUAL 1)
  GET_ALL_TARGETS_FAMILY(eth_cmd_family_files ${cmd_zoo_path})
  set(COMPILE_WITH_OPENCV 1)
else()
  set(cmd_subdirs_family_list "")
  if(${USE_ETH_CMD_UNCOMPRESS_FRAME_DEFAULT} EQUAL 1)
  list(APPEND cmd_subdirs_family_list "uncompress_frame")
  endif()
  if(${USE_ETH_CMD_COPTER_ALARM} EQUAL 1)
  list(APPEND cmd_subdirs_family_list "copter_alarm")
  endif()
  if(${USE_ETH_CMD_TELEMETRY_ATAS} EQUAL 1)
  list(APPEND cmd_subdirs_family_list "telemetry_atas")
  endif()
  if(${USE_ETH_CMD_POWEROFF_REQUEST} EQUAL 1)
    list(APPEND cmd_subdirs_family_list "poweroff_request")
  endif()

  if(${USE_ETH_CMD_AP_TELEMETRY} EQUAL 1)
    list(APPEND cmd_subdirs_family_list "ap_telemetry")
  endif()
  if(${USE_ETH_CMD_TRACKER_METADATA} EQUAL 1)
    list(APPEND cmd_subdirs_family_list "track_meta")
  endif()
  if(${USE_ETH_CMD_TELEMETRY_ORIENTATION} EQUAL 1)
    list(APPEND cmd_subdirs_family_list "telemetry_orientation")
  endif()
  
  if(${USE_ETH_CMD_AP_POS_CNT} EQUAL 1)
    list(APPEND cmd_subdirs_family_list "ap_pos_cnt")
  endif()
  
  if(${USE_ETH_CMD_FENIX2META} EQUAL 1)
    list(APPEND cmd_subdirs_family_list "fenix2meta")
  endif()
  if(${USE_ETH_CMD_FENIX2CMD} EQUAL 1)
    list(APPEND cmd_subdirs_family_list "fenix2cmd")
  endif()

  if(${USE_ETH_CMD_400_INIT} EQUAL 1)
    list(APPEND cmd_subdirs_family_list "cmd400")
  endif()
  if(${USE_ETH_CMD_400_DEINIT} EQUAL 1)
    list(APPEND cmd_subdirs_family_list "cmd400")
  endif()
  if(${USE_ETH_CMD_RECORD_ON} EQUAL 1)
    list(APPEND cmd_subdirs_family_list "cmd400")
  endif()

  if(${USE_ETH_CMD_VERTELKA_CLOSE} EQUAL 1)
      list(APPEND cmd_subdirs_family_list "cmd_vertelka")
  endif()

  if(${USE_ETH_CMD_VERTELKA_RUN10} EQUAL 1)
      list(APPEND cmd_subdirs_family_list "cmd_vertelka")
  endif()

  if(${USE_ETH_CMD_VERTELKA_RUN8} EQUAL 1)
      list(APPEND cmd_subdirs_family_list "cmd_vertelka")
  endif()


  GET_SELECTED_TARGETS_FAMILY(eth_cmd_family_files ${cmd_zoo_path} "${cmd_subdirs_family_list}") # передача списка в кавычках необходима, иначе передается только первый элемент
  # message("eth_cmd_family_files = " ${eth_cmd_family_files})
endif()
target_sources(eth_controller_toolbox_lib PRIVATE ${eth_cmd_family_files})

target_link_libraries(eth_controller_toolbox_lib -lpthread)

if(${COMPILE_WITH_OPENCV} EQUAL 1)
  target_link_libraries(eth_controller_toolbox_lib ${OpenCV_LIBS} )
endif()

if(WIN32)
target_link_libraries(eth_controller_toolbox_lib -lws2_32)
endif()

set(TEST_DIR_PATH "${CMAKE_CURRENT_LIST_DIR}/tests/")

if(${COMPILE_ALL_TESTS} EQUAL 1)
  set(USE_TEST_ASYNC_UDP_CLIENT                 1)
  set(USE_TEST_ASYNC_UDP_SERVER                 1)
  set(USE_TEST_MANUAL_ATAS_TELEMETRY            1)
  set(USE_TEST_MANUAL_CONFIRMATION_CMD_RECEIVE  1)
  set(USE_TEST_SETIALIZATION_STRING             1)
  set(USE_TEST_TABLE_ID_REPLACE                 1)
  set(USE_TEST_RCV_ORIENT_TELE                  1)
  set(USE_TEST_RCV_AP_POS_CNT                   1)
  set(USE_TEST_SND_AP_POS_CNT                   1)
  set(USE_TEST_BASIC_UDP_SERVER                 1)
  set(USE_TEST_BASIC_UDP_CLIENT                 1)
  set(USE_TEST_CHRONO_NEGATIVE_TIME             1)
  set(USE_TEST_GSTC_RTPX264_SERVER              1)
  set(USE_TEST_GSTC_RTPX264_CLIENT              1)


endif()

if(${USE_TEST_ASYNC_UDP_CLIENT} EQUAL 1)
  add_executable(test_async_udp_client 
    ${TEST_DIR_PATH}/manual/test_async_udp_client/test_async_udp_client.cpp)
    target_link_libraries(test_async_udp_client eth_controller_toolbox_lib)
endif()

if(${USE_TEST_ASYNC_UDP_SERVER} EQUAL 1)
add_executable(test_async_udp_server 
  ${TEST_DIR_PATH}/manual/test_async_udp_server/test_async_udp_server.cpp)
    target_link_libraries(test_async_udp_server eth_controller_toolbox_lib)
endif()

if(${USE_TEST_MANUAL_ATAS_TELEMETRY} EQUAL 1)
  add_executable(test_manual_atas_telemetry 
    "${TEST_DIR_PATH}/manual/telemetry_atas_cmd/test_telemetry_atas_cmd_eq_pack_unpack.cpp")
  target_link_libraries(test_manual_atas_telemetry eth_controller_toolbox_lib)
endif()

if(${USE_TEST_MANUAL_CONFIRMATION_CMD_RECEIVE} EQUAL 1)
  add_executable(test_manual_confirmation_cmd_receive 
    "${TEST_DIR_PATH}/manual/confirmation_cmd_receive/test_confirmation_cmd_receive.cpp")
  target_link_libraries(test_manual_confirmation_cmd_receive eth_controller_toolbox_lib)
endif()

if(${USE_TEST_SETIALIZATION_STRING} EQUAL 1)
  add_executable(test_serialization_string "${TEST_DIR_PATH}/manual/test_serialization_string/test_serialization_string.cpp")
  target_link_libraries(test_serialization_string eth_controller_toolbox_lib)
endif()

if(${USE_TEST_TABLE_ID_REPLACE} EQUAL 1)
  add_executable(test_table_id_replace 
    "${TEST_DIR_PATH}/manual/table_id_replace/test_tir.cpp")
  target_link_libraries(test_table_id_replace eth_controller_toolbox_lib)
endif()

if(${USE_TEST_RCV_ORIENT_TELE} EQUAL 1)
  add_executable(test_rcv_orient_tele "${TEST_DIR_PATH}/manual/test_rcv_orient_tele/test_rcv_orient_tele.cpp")
  target_link_libraries(test_rcv_orient_tele eth_controller_toolbox_lib)
endif()

if(${USE_TEST_RCV_AP_POS_CNT} EQUAL 1)
  add_executable(test_rcv_ap_pos_cnt 
  ${TEST_DIR_PATH}/manual/test_rcv_ap_pos_cnt/test_rcv_ap_pos_cnt.cpp
  ${TEST_DIR_PATH}/manual/test_rcv_ap_pos_cnt/settings.ini
  )
  target_link_libraries(test_rcv_ap_pos_cnt eth_controller_toolbox_lib)
endif()

if(${USE_TEST_SND_AP_POS_CNT} EQUAL 1)
  add_executable(test_snd_ap_pos_cnt 
  ${TEST_DIR_PATH}/manual/test_snd_ap_pos_cnt/test_snd_ap_pos_cnt.cpp
  ${TEST_DIR_PATH}/manual/test_snd_ap_pos_cnt/settings.ini
  )
  target_link_libraries(test_snd_ap_pos_cnt eth_controller_toolbox_lib)
endif()


if(${USE_TEST_BASIC_UDP_SERVER} EQUAL 1)
  add_executable(test_basic_udp_server
  ${TEST_DIR_PATH}/manual/basic_udp_server/test_basic_udp_server.cpp
  )
  target_link_libraries(test_basic_udp_server eth_controller_toolbox_lib)
endif()

if(${USE_TEST_BASIC_UDP_CLIENT} EQUAL 1)
  add_executable(test_basic_udp_client
  ${TEST_DIR_PATH}/manual/basic_udp_client/test_basic_udp_client.cpp
  )
  target_link_libraries(test_basic_udp_client eth_controller_toolbox_lib)
endif()

if(${USE_TEST_CHRONO_NEGATIVE_TIME} EQUAL 1)
  add_executable(test_chrono_negative_time
  ${TEST_DIR_PATH}/manual/chrono_features/negative_time/negative_time.cpp
  )
endif()

if(${USE_TEST_GSTC_RTPX264_SERVER} EQUAL 1)
  add_executable(test_gstc_x264_server 
  ${TEST_DIR_PATH}/manual/test_rtpx264_server/test_rtpx264_server.cpp
  )
  target_link_libraries(test_gstc_x264_server eth_controller_toolbox_lib)
endif()

if(${USE_TEST_GSTC_RTPX264_CLIENT} EQUAL 1)
  add_executable(test_gstc_x264_client
  ${TEST_DIR_PATH}/manual/test_rtpx264_client/test_rtpx264_client.cpp
  )
  target_link_libraries(test_gstc_x264_client eth_controller_toolbox_lib)
endif()


if(${COMPILE_ALL_APPS})
  set(USE_APP_RETRANSLATOR_TELE_ORIENT_TO_AP 1)
endif()

if(${USE_APP_RETRANSLATOR_TELE_ORIENT_TO_AP} EQUAL 1)
  add_executable(app_retranslator_tele_orient_to_ap 
    apps/app_retransmitter_telemetry_orient2ap/app_retransmitter_telemetry_orient2ap.cpp
    apps/app_retransmitter_telemetry_orient2ap/retransmitter_tele_orient2ap_config.ini
    )
  target_link_libraries(app_retranslator_tele_orient_to_ap eth_controller_toolbox_lib)
endif()

if(${USE_VIDEO_DEMO} EQUAL 1)
  add_executable(demo_srt_gstcv_client demo/demo_srt_gstcv_client.cpp)
  target_link_libraries(demo_srt_gstcv_client eth_controller_toolbox_lib)

  add_executable(demo_srt_gstcv_server demo/demo_srt_gstcv_server.cpp)
  target_link_libraries(demo_srt_gstcv_server eth_controller_toolbox_lib)
endif()
