add_executable(ldc20i-control
    main.cpp
    ldc20i/ldc20i_controller.cpp
    ldc20i/ldc20i_controller.h
    ldc20i/ldc20i_qserial.cpp
    ldc20i/ldc20i_qserial.h
    ldc20i/ldc20i_worker.cpp
    ldc20i/ldc20i_worker.h
)

target_link_libraries(ldc20i-control PRIVATE
    Qt6::Core
    Qt6::SerialPort
)

target_compile_features(ldc20i-control PRIVATE cxx_std_17)

configure_file(
    "${PROJECT_SOURCE_DIR}/../prebuild/config.ini"
    "${PROJECT_BINARY_DIR}/config.ini"
    COPYONLY
)

include(GNUInstallDirs)
install(TARGETS ldc20i-control RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES "${PROJECT_SOURCE_DIR}/../prebuild/config.ini"
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        RENAME config.ini)
