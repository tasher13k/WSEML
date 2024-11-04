add_executable(wseml_code src/wseml7.cpp src/debug7.cpp
        ) #  src/locale.cpp не забыть src/debug7.cpp и src/examples.cpp


include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_include_directories(wseml_code PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

message("NEED_LIB_WSCODE = ${NEED_LIB_W}")
target_link_libraries(wseml_code PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(wseml_code PRIVATE wseml_class_code)

#get_property(dirs TARGET wseml_code PROPERTY INCLUDE_DIRECTORIES)
#foreach(dir ${dirs})
#    message(STATUS "dir='${dir}'")
#endforeach()