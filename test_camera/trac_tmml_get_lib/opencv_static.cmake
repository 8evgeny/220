#это файл для сборки со статическим opencv, собранным с помощью
#-DBUILD_SHARED_LIBS=OFF, -DOPENCV_GENERATE_PKGCONFIG=ON
#пример использования файла:
#
#set(OpenCV_DIR ${project_DIR}opencv/install_qt_4.1.2)
#include(${CMAKE_CURRENT_LIST_DIR}/opencv_static.cmake)
#
#экспортирует интерфейсные библиотеки OPENCV_STATIC_GUI и OPENCV_STATIC с ними можно линковаться

set(CMAKE_PREFIX_PATH ${OpenCV_DIR})
find_package(PkgConfig REQUIRED)
pkg_search_module(Pkg_OpenCV REQUIRED IMPORTED_TARGET opencv4)
include_directories(${Pkg_OpenCV_INCLUDE_DIRS})
message("Pkg_OpenCV_STATIC_LIBS=${Pkg_OpenCV_STATIC_LIBS}")
message("Pkg_OpenCV_STATIC_LIBRARIES=${Pkg_OpenCV_STATIC_LIBRARIES}")
message("Pkg_OpenCV_STATIC_DIRS=${Pkg_OpenCV_STATIC_DIRS}")
message("Pkg_OpenCV_STATIC_LIBRARY_DIRS=${Pkg_OpenCV_STATIC_LIBRARY_DIRS}")
#link_directories(${Pkg_OpenCV_STATIC_LIBRARY_DIRS})

add_library(OPENCV_STATIC_GUI INTERFACE)
target_include_directories(OPENCV_STATIC_GUI INTERFACE ${Pkg_OpenCV_INCLUDE_DIRS})
target_link_libraries(OPENCV_STATIC_GUI INTERFACE ${Pkg_OpenCV_STATIC_LIBRARIES};-lgomp)
link_directories(OPENCV_STATIC_GUI INTERFACE ${Pkg_OpenCV_STATIC_LIBRARY_DIRS})

add_library(OPENCV_STATIC INTERFACE)
target_include_directories(OPENCV_STATIC INTERFACE ${Pkg_OpenCV_INCLUDE_DIRS})
set(OPENCV_STATIC_LIBRARIES ${Pkg_OpenCV_STATIC_LIBRARIES})
list(REMOVE_ITEM OPENCV_STATIC_LIBRARIES opencv_highgui)

list(REMOVE_ITEM OPENCV_STATIC_LIBRARIES Qt5Core)
list(REMOVE_ITEM OPENCV_STATIC_LIBRARIES Qt5Gui)
list(REMOVE_ITEM OPENCV_STATIC_LIBRARIES Qt5Widgets)
list(REMOVE_ITEM OPENCV_STATIC_LIBRARIES Qt5Test)
list(REMOVE_ITEM OPENCV_STATIC_LIBRARIES Qt5Concurrent)
message("OPENCV_STATIC_LIBRARIES=${OPENCV_STATIC_LIBRARIES}")
target_link_libraries(OPENCV_STATIC INTERFACE ${OPENCV_STATIC_LIBRARIES};-lgomp)
link_directories(OPENCV_STATIC INTERFACE ${Pkg_OpenCV_STATIC_LIBRARY_DIRS})

