cmake_minimum_required(VERSION 3.16)

project(libGoen220Data LANGUAGES CXX VERSION 1.0.0)

add_compile_options(-fpermissive)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_AR "/usr/bin/aarch64-linux-gnu-ar")
#set(CMAKE_RANLIB "/usr/bin/aarch64-linux-gnu-ranlib")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core
    PATHS "${CMAKE_PREFIX_PATH}"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core
    PATHS "${CMAKE_PREFIX_PATH}"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)

set(src_dir src)

set(src_list
  ${src_dir}/goen220_utils.h
  ${src_dir}/goen220_types.h
  ${src_dir}/goen220_cmd.h
  ${src_dir}/goen220_tlm.h
  ${src_dir}/goen220_tlm_data_model.h
  ${src_dir}/libgoen220data.h
  ${src_dir}/libgoen220data.cpp
  ${src_dir}/_internal/tlm_data_model_impl.h
  ${src_dir}/_internal/tlm_data_model_impl.cpp
  ${src_dir}/_internal/tlm_data_category_filter_model.h
  ${src_dir}/_internal/tlm_data_category_filter_model.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${src_list})

add_library(libGoen220Data STATIC ${src_list})
set_property(TARGET libGoen220Data PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(libGoen220Data PUBLIC src)

target_link_libraries(libGoen220Data PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(libGoen220Data PUBLIC LIB_GOEN220_DATA_VERSION="${PROJECT_VERSION}")
target_compile_definitions(libGoen220Data PUBLIC LIB_GOEN220_DATA_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
target_compile_definitions(libGoen220Data PUBLIC LIB_GOEN220_DATA_VERSION_MINOR=${PROJECT_VERSION_MINOR})
target_compile_definitions(libGoen220Data PUBLIC LIB_GOEN220_DATA_VERSION_PATCH=${PROJECT_VERSION_PATCH})

target_compile_definitions(libGoen220Data PRIVATE LIBGOEN220_DATA_LIBRARY)
