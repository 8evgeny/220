cmake_minimum_required(VERSION 3.10)

# Название вашего проекта и используемые языки
project(LdcLaserDriverProject LANGUAGES CXX)

# Включаем проверку ОС: код написан строго под Linux/POSIX API
if(NOT CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(FATAL_ERROR "Этот драйвер использует POSIX терминилы (termios.h) и поддерживает только ОС Linux!")
endif()

# Установка стандарта C++17 (или C++11 / C++20 при необходимости)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Оптимизация компиляции (Release режим включает -O3, Debug — для отладки gdb)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Опции компилятора: выводить все предупреждения (-Wall -Wextra) для чистоты кода
add_compile_options(-Wall -Wextra -O3)

# Добавление исполняемого файла
# Предполагается, что ваш код сохранен под именем main.cpp
add_executable(ldc_driver main.cpp)

# Если в будущем вы вынесете работу с потоками в std::thread, 
# CMake автоматически найдет и привяжет системную библиотеку pthread:
find_package(Threads REQUIRED)
target_link_libraries(ldc_driver PRIVATE Threads::Threads)

# Вывод информации о сборке в терминал
message(STATUS "Проект настроен для сборки в режиме: ${CMAKE_BUILD_TYPE}")
