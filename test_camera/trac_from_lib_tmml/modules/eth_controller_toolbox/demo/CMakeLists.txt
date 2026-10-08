find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(demo_cli_x264_gray client_x264_gray.cpp)
target_link_libraries(demo_cli_x264_gray ${OpenCV_LIBS})

add_executable(demo_srv_x264_gray server_x264_gray.cpp)
target_link_libraries(demo_srv_x264_gray ${OpenCV_LIBS})

