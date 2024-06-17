cmake_minimum_required(VERSION 3.27)
project(wseml_class_code)

set(SRC_FILES src/WSEML.cpp src/parser.cpp src/pointers.cpp src/misc.cpp src/helpFunc.cpp) #без main.cpp
set(INCL_FILES include/WSEML.h include/parser.h include/pointers.h include/dllconfig.h include/misc.h include/helpFunc.h include/lists.h
        include/stringconverter.h)

if (WIN32)
    #add_executable(wseml_class_code main.cpp ${SRC_FILES} ${INCL_FILES} src/arch/win32/funcCall.cpp)
    add_library(wseml_class_code ${SRC_FILES} ${INCL_FILES} src/arch/win32/funcCall.cpp
            include/stringconverter.h)
endif (WIN32)
if (UNIX)
    #add_executable(wseml_class_code main.cpp ${SRC_FILES} ${INCL_FILES} src/arch/unix/funcCall.cpp)
    add_library(wseml_class_code ${SRC_FILES} ${INCL_FILES} src/arch/unix/funcCall.cpp
            include/stringconverter.h)
endif (UNIX)

target_include_directories(wseml_class_code PRIVATE "${CMAKE_HOME_DIRECTORY}/lib_gmp-6.3.0/include")
target_include_directories(wseml_class_code PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

#message("Current_src_dir = ${CMAKE_CURRENT_SOURCE_DIR} ; PROJ_src_dir = ${PROJECT_SOURCE_DIR}  List_of_dirs =  ${CMAKE_CURRENT_LIST_DIR}")

target_link_libraries(wseml_class_code "${CMAKE_HOME_DIRECTORY}/lib_gmp-6.3.0/lib/libgmpxx.a")
target_link_libraries(wseml_class_code "${CMAKE_HOME_DIRECTORY}/lib_gmp-6.3.0/lib/libgmp.a")