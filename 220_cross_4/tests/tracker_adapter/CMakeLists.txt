# Тесты модуля vdb_vrt_tracker_adapter (Qt не требуется).
# cmake -B build && cmake --build build && ./build/test_tracker_adapter
cmake_minimum_required(VERSION 3.16)
project(test_tracker_adapter CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ADAPTER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app/gpf/tracker_adapter)

add_executable(test_tracker_adapter
    test_tracker_adapter.cpp
    ${ADAPTER_DIR}/vdb_vrt_tracker_adapter_impl.cpp
    ${ADAPTER_DIR}/pid_regulator.cpp)

target_include_directories(test_tracker_adapter PRIVATE ${ADAPTER_DIR})

enable_testing()
add_test(NAME tracker_adapter COMMAND test_tracker_adapter)
