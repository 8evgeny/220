cmake_minimum_required(VERSION 3.16)

project(CrossCompileTest VERSION 1.0 LANGUAGES CXX)

# Автоматизация Qt (moc, uic, rcc)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск компонентов Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Создание исполняемого файла
add_executable(CrossCompileTest main.cpp)

# Линк с библиотеками Qt
target_link_libraries(CrossCompileTest PRIVATE Qt6::Widgets)
