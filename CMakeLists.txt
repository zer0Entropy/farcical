cmake_minimum_required(VERSION 3.28)
project(farcical LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(SFML json)

add_executable( main
        src/color.cpp
        src/engine/event.cpp
        src/engine/input.cpp
        src/engine/log.cpp
        src/engine/logInterface.cpp
        src/engine/render.cpp
        src/resource/parser.cpp
        src/resource/manager.cpp
        src/ui/config.cpp
        src/ui/root.cpp
                src/ui/menu.cpp
                src/ui/label.cpp
                src/ui/button.cpp
        src/ui/decoration.cpp
        src/ui/scene.cpp
        src/engine/config.cpp
        src/engine/engine.cpp
        src/game/game.cpp
        src/game/map.cpp
        src/main.cpp
        src/resource/parser.cpp
        src/resource/config.cpp
        src/ui/factory.cpp
)
target_compile_features(main PRIVATE cxx_std_23)
target_link_libraries(main PRIVATE SFML::Graphics nlohmann_json::nlohmann_json)
