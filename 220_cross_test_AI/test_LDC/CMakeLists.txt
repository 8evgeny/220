cmake_minimum_required(VERSION 3.16)

project(test_LDC LANGUAGES CXX)

if(CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "test_LDC is a Desktop-only project; configure it with a Desktop kit.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core SerialPort)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.ini"
    "${CMAKE_CURRENT_BINARY_DIR}/config.ini"
    COPYONLY
)

add_executable(test_LDC main.cpp)
target_link_libraries(test_LDC PRIVATE Qt6::Core Qt6::SerialPort)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set_target_properties(test_LDC PROPERTIES
        BUILD_RPATH "$<TARGET_FILE_DIR:Qt6::Core>"
    )
    target_link_options(test_LDC PRIVATE "-Wl,--disable-new-dtags")
endif()

include(GNUInstallDirs)
install(TARGETS test_LDC RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/config.ini"
        DESTINATION ${CMAKE_INSTALL_BINDIR})
