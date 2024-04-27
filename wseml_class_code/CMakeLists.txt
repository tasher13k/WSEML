cmake_minimum_required(VERSION 3.17)
project(wseml_class_code)

set(CMAKE_CXX_STANDARD 14)
include_directories("${CMAKE_HOME_DIRECTORY}/lib_gmp-6.3.0/include")

if (WIN32)
    add_executable(wseml_class_code main.cpp include/WSEML.h src/WSEML.cpp src/parser.cpp include/parser.h include/pointers.h src/pointers.cpp include/dllconfig.h include/misc.h src/misc.cpp src/arch/win32/funcCall.cpp src/helpFunc.cpp include/helpFunc.h include/lists.h)
endif (WIN32)
if (UNIX)
    add_executable(wseml_class_code main.cpp include/WSEML.h src/WSEML.cpp src/parser.cpp include/parser.h include/pointers.h src/pointers.cpp include/dllconfig.h include/misc.h src/misc.cpp src/arch/unix/funcCall.cpp src/helpFunc.cpp include/helpFunc.h include/lists.h)
endif (UNIX)
target_link_libraries(wseml_class_code "${CMAKE_SOURCE_DIR}/lib_gmp-6.3.0/lib/libgmpxx.a")
target_link_libraries(wseml_class_code "${CMAKE_SOURCE_DIR}/lib_gmp-6.3.0/lib/libgmp.a")