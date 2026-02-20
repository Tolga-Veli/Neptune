cmake_minimum_required(VERSION 3.10)
project(chess)

set(CMAKE_CXX_STANDARD 20)
add_compile_definitions(PROJ_DIR="${CMAKE_SOURCE_DIR}")

# GLAD
add_library(glad libs/glad/src/glad.c)
target_include_directories(glad PUBLIC libs/glad/include)

#add_subdirectory(libs/stb)
#target_include_directories(stb_image PUBLIC libs/stb)

# GLM
add_subdirectory(libs/glm)

# ImGui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/libs/imgui)
file(GLOB IMGUI_SRC
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui ${IMGUI_SRC})
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    libs/glfw/include
)

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

add_subdirectory(libs/glfw)

# Executable
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(chess ${SRC_FILES})

target_include_directories(chess PUBLIC src)
target_link_libraries(chess glfw glad imgui ${CMAKE_DL_LIBS})