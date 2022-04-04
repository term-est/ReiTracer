cmake_minimum_required(VERSION 3.21)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()


project(RayTracer)

set(CMAKE_CXX_STANDARD 23)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(imgui REQUIRED)
find_package(SDL2 REQUIRED)

add_executable(RayTracer RayTracer.cpp backends/imgui_impl_sdl.cpp backends/imgui_impl_sdlrenderer.cpp)
target_link_libraries(RayTracer PRIVATE fmt::fmt spdlog::spdlog mingw32 imgui::imgui SDL2::SDL2main SDL2::SDL2)
