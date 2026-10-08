cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)
project(trac_tmml_get_lib)

set(FROM_LIB ${CMAKE_CURRENT_LIST_DIR}/../trac_from_lib_tmml_multitrack)
#set(FROM_LIB ${CMAKE_CURRENT_LIST_DIR}/../trac_from_lib_tmml)
#set(FROM_LIB ${CMAKE_CURRENT_LIST_DIR}/../../fenix2/fenix2_from_lib)

set(USE_GPU 2) # (0 - CPU, 1 - CUDA, 2 - OPENCL)

# Тип корреляции при template matching
# CORR_TYPE = 2 : SQDIFF_NORMED (Прямая корреляция)
# CORR_TYPE = 8 ; COMBINED (объединение прямой и модифицированной кросскорреляции)
set(CORR_TYPE 8)

set(TEMPLATE_WIDTH 48)
set(EXT_VAL 5)

set(OpenCV_DIR $ENV{HOME}/opencv/install_4_10_0_contrib_cuda_static)
include_directories($CMAKE_SOURCE_DIR})

set(COUT_OK 0) # Вывод в терминал.
set(MAX_FPS_OK 0) # Включение/выключение задержки.
set(FIND_DISC_ID 0) # Привязка к железу вычислителя.
set(KOEFF2LIB 1) # Коэффициент для получения 8-й либы (при вычитании из 6-й либы 2-й либы).
                 # Если KOEFF2LIB=0, то используется 6-я библиотека (CCOEFF_NORMED  (Кросскорреляция модифицированная)) - CORR_TYPE = 6 - удалена

set(USE_img_et_sm 0) # Для режима GPU=0 это полезная вещь.
set(USE_scale 0) # Включение масштабирования.
set(USE_smooth 0) # Включение масштабирования.

#######################################################################

add_definitions(-DKOEFF2LIB=${KOEFF2LIB})

if(${USE_img_et_sm} EQUAL 1)
    add_definitions(-DUSE_img_et_sm)
endif() # END if(${USE_img_et_sm} EQUAL 1)

if(${USE_scale} EQUAL 1)
    add_definitions(-DUSE_scale)
endif() # END if(${USE_scale} EQUAL 1)

if(${USE_smooth} EQUAL 1)
    add_definitions(-DUSE_smooth)
endif() # END if(${USE_smooth} EQUAL 1)

if(${USE_GPU} EQUAL 0)
    add_definitions(-DNO_GPU)
    message(STATUS "--------------------------- NO_GPU")
    set(lib_name _trac_tmml_${CORR_TYPE}_CPU)
endif() # END if(${USE_GPU} EQUAL 0)

if(${USE_GPU} EQUAL 1)
 add_definitions(-DUSE_CUDA)
 set(lib_name _trac_tmml_${CORR_TYPE}_CUDA)
 set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xptxas -O1 ")
endif() # END if(${USE_GPU} EQUAL 1)

if(${USE_GPU} EQUAL 2)
 add_definitions(-DOPEN_CL)
 set(lib_name _trac_tmml_${CORR_TYPE}_CL)
 find_package(OpenCL REQUIRED)
 include_directories (${OpenCL_INCLUDE_DIRS})
endif() # END if(${USE_GPU} EQUAL 2)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${FROM_LIB}/lib)
include(${CMAKE_CURRENT_LIST_DIR}/opencv_static.cmake)

find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)
include_directories(${OpenCV_DIR})

set(threads_num $(nproc))
set(CMAKE_MAKE_PROGRAM "${CMAKE_MAKE_PROGRAM} -j${threads_num}")

if(${CORR_TYPE} EQUAL 2)
    add_definitions(-DSQDIFF_NORMED)
endif() # END if(${CORR_TYPE} EQUAL 2)
if(${CORR_TYPE} EQUAL 6)
    add_definitions(-DCCOEFF_NORMED)
    add_definitions(-DCORR_TYPE=TM_CCOEFF_NORMED)
endif() # END if(${CORR_TYPE} EQUAL 6)
if(${CORR_TYPE} EQUAL 8)
    add_definitions(-DCOMBINED)
endif() # END if(${CORR_TYPE} EQUAL 8)

if(${USE_GPU} EQUAL 1)
    set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)
    option(USE_CUDA "Use CUDA" ON)
    enable_language("CUDA")
    find_package(CUDA REQUIRED)
    include_directories(${CUDA_INCLUDE_DIRS})
    set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)
    option(USE_CUDA "Use CUDA" ON)
    enable_language("CUDA")
    find_package(CUDA REQUIRED)
    include_directories(${CUDA_INCLUDE_DIRS})

#    #Get CUDA compute capability
#    include(FindCUDA/select_compute_arch)
#    CUDA_DETECT_INSTALLED_GPUS(INSTALLED_GPU_CCS_1)
#    string(STRIP "${INSTALLED_GPU_CCS_1}" INSTALLED_GPU_CCS_2)
#    string(REPLACE " " ";" INSTALLED_GPU_CCS_3 "${INSTALLED_GPU_CCS_2}")
#    string(REPLACE "." "" CUDA_ARCH_LIST "${INSTALLED_GPU_CCS_3}")
#    SET(CMAKE_CUDA_ARCHITECTURES ${CUDA_ARCH_LIST})
#    message(STATUS "=================CUDA_ARCH_LIST:${CUDA_ARCH_LIST}")
#    string(SUBSTRING ${CUDA_ARCH_LIST} 0 1 ccc_major)
#    string(SUBSTRING ${CUDA_ARCH_LIST} 1 1 ccc_minor)
#    message(STATUS "=================ccc_major:${ccc_major}")
#    message(STATUS "=================ccc_minor:${ccc_minor}")

#    if(${ccc_major} STREQUAL "8")
#        add_definitions(-DGPU_AMPERE)
#        message(STATUS "GPU_AMPERE (видеокарты серии 30ХХ)")
#    elseif(${ccc_major} STREQUAL "7")
#        add_definitions(-DGPU_VOLTA)
#        message(STATUS "GPU_VOLTA (XAVIER NX)")
#    else()
#        add_definitions(-DGPU_AMPERE)
#        message(STATUS "GPU автоматически не определилось - установлено GPU_AMPERE")
#    endif() # END if(!${ccc_major} STREQUAL "8")
endif() # END if(${USE_GPU} EQUAL 1)

if(${TEMPLATE_WIDTH})
    add_definitions(-DTEMPLATE_WIDTH=${TEMPLATE_WIDTH})
endif() # END if(${TEMPLATE_WIDTH})

if(${EXT_VAL})
    add_definitions(-DEXT_VAL=${EXT_VAL})
endif() # END if(${EXT_VAL})

if(${FIND_DISC_ID} EQUAL 1)
  add_definitions(-DFIND_DISC_ID)
  message("----- FIND_DISC_ID enable")
endif() # END if(${FIND_DISC_ID} EQUAL 1)

if(${COUT_OK} EQUAL 1)
  add_definitions(-DCOUT_OK)
  message("----- COUT_OK enable")
endif() # END if(${COUT_OK} EQUAL 1)

add_definitions(-DTRACK_API="${FROM_LIB}/tracshats/trac_api.hpp")

link_directories(${CMAKE_CURRENT_LIST_DIR})

# Алгоритм средней скорости _lib_trac_tmml (генерация LIB):
if(${USE_GPU} EQUAL 0)
    add_library(${lib_name} SHARED
        trac_scale.hpp
        trac_scale.cpp
        ${FROM_LIB}/tracshats/trac_api.hpp
        ${FROM_LIB}/tracshats/trac_struct.hpp
        tmml.cpp
        tmml.hpp
     )
endif() # END if(${USE_GPU} EQUAL 0)

if(${USE_GPU} EQUAL 1)
    add_library(${lib_name} SHARED
        trac_scale.hpp
        trac_scale.cpp
        ${FROM_LIB}/tracshats/trac_api.hpp
        ${FROM_LIB}/tracshats/trac_struct.hpp
        tmml.cpp
        tmml.cu
        tmml.hpp
    )
    set_target_properties(${lib_name} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif() # END if(${USE_GPU} EQUAL 1)

if(${USE_GPU} EQUAL 2)
    add_library(${lib_name} SHARED
        trac_scale.hpp
        trac_scale.cpp
        ${FROM_LIB}/tracshats/trac_api.hpp
        ${FROM_LIB}/tracshats/trac_struct.hpp
        tmml_cl.cpp
        cl.cpp
        tmml_cl.hpp
    )
    target_link_libraries(${lib_name} ${OpenCL_LIBRARY})
endif() # END if(${USE_OPEN_CL} EQUAL 2)

target_link_libraries(${lib_name} PRIVATE)


