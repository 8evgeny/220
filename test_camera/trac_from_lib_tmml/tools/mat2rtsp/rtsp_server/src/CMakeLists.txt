add_executable(mat2rtsp_bin gst-rtsp-launch.cpp)
target_link_libraries(mat2rtsp_bin PUBLIC MAT2RTSP)

install(TARGETS mat2rtsp_bin
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
)
