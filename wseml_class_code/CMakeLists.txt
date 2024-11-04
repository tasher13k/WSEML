cmake_minimum_required(VERSION 3.27)
project(wseml_class_code)

set(SRC_FILES src/WSEML.cpp src/parser.cpp src/pointers.cpp src/misc.cpp src/helpFunc.cpp main.cpp) #без main.cpp
set(INCL_FILES include/WSEML.h include/parser.h include/pointers.h include/dllconfig.h include/misc.h include/helpFunc.h include/lists.h
        include/stringconverter.h)

if (WIN32)
    if (NEED_LIB_W)
        add_library(wseml_class_code ${SRC_FILES} ${INCL_FILES} src/arch/win32/funcCall.cpp
                include/stringconverter.h)
    else ()
        add_executable(wseml_class_code main.cpp ${SRC_FILES} ${INCL_FILES} src/arch/win32/funcCall.cpp)
    endif ()
endif (WIN32)
if (UNIX)
    if (NEED_LIB_W)
        add_library(wseml_class_code ${SRC_FILES} ${INCL_FILES} src/arch/unix/funcCall.cpp
                include/stringconverter.h)
    else ()
        add_executable(wseml_class_code main.cpp ${SRC_FILES} ${INCL_FILES} src/arch/unix/funcCall.cpp)
    endif()
endif (UNIX)

target_include_directories(wseml_class_code PRIVATE "${CMAKE_HOME_DIRECTORY}/lib_gmp-6.3.0/include")
target_include_directories(wseml_class_code PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

message("NEED_LIB_WSCLAS_CODE = ${NEED_LIB_W}")

target_link_libraries(wseml_class_code "${CMAKE_HOME_DIRECTORY}/lib_gmp-6.3.0/lib/libgmpxx.a")
target_link_libraries(wseml_class_code "${CMAKE_HOME_DIRECTORY}/lib_gmp-6.3.0/lib/libgmp.a")