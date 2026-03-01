cmake_minimum_required(VERSION 3.25)
project(Neptune LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# SFML Options
option(SFML_BUILD_AUDIO "Build audio" OFF)
option(SFML_BUILD_NETWORK "Build network" OFF)
option(SFML_BUILD_EXAMPLES "Build examples" OFF)
option(SFML_BUILD_DOC "Build documentation" OFF)

add_subdirectory(libs/sfml)

# Tell imgui-sfml where imgui is
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)
option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)
option(IMGUI_SFML_IMGUI_DEMO "Build imgui_demo.cpp" ON)

add_subdirectory(libs/imgui-sfml)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.hpp")
add_executable(Neptune ${SOURCES} ${HEADERS})

target_include_directories(Neptune PRIVATE src)

target_link_libraries(Neptune
    PRIVATE
        SFML::Graphics
        ImGui-SFML::ImGui-SFML
)