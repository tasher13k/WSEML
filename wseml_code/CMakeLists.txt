add_executable(wseml_code src/wseml7.cpp src/debug7.cpp) # не забыть src/main7.cpp и src/examples.cpp

target_include_directories(wseml_code PUBLIC ./includes)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(wseml_code PRIVATE nlohmann_json::nlohmann_json)