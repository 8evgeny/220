add_executable(pid_tests
    PidTest.cpp
    ../app/gpf/Pid.cpp
)

target_include_directories(pid_tests PRIVATE
    ../app/gpf
)

target_compile_features(pid_tests PRIVATE cxx_std_17)

if(NOT CMAKE_CROSSCOMPILING OR CMAKE_CROSSCOMPILING_EMULATOR)
    add_test(NAME pid_tests COMMAND pid_tests)
else()
    message(STATUS "PID tests are built but not registered: configure CMAKE_CROSSCOMPILING_EMULATOR to run them")
endif()
