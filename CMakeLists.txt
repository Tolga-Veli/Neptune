cmake_minimum_required(VERSION 3.10)
project(Neptune VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)

add_subdirectory(libs/sfml)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp")

add_executable(Neptune ${SOURCES} ${HEADERS})

target_include_directories(Neptune PRIVATE src)

target_link_libraries(Neptune sfml-graphics sfml-window sfml-system)

