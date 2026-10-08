add_executable(rtsp_server_device gst-rtsp-launch.cpp)
target_link_libraries(rtsp_server_device PUBLIC GST_RTSP_SERVER)

install(TARGETS rtsp_server_device
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
)
