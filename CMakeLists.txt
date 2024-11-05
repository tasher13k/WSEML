cmake_minimum_required(VERSION 3.27)
project(WSEML)

set(CMAKE_CXX_STANDARD 14)

set(NEED_LIB_W OFF)

add_subdirectory(wseml_class_code)
if (NEED_LIB_W)
    add_subdirectory(wseml_code)
endif ()


