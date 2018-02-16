cmake_minimum_required(VERSION 3.9)
project(pong_opengl)

set(CMAKE_CXX_STANDARD 14)

#use glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(dependencies/glfw)

#use glew
add_subdirectory(dependencies/glew/build/cmake)

if(APPLE)
    find_library(cocoa_lib Cocoa)
    find_library(iokit_lib IOKit)
    find_library(opengl_lib OpenGL)

    set(framework
            ${cocoa_lib}
            ${iokit_lib}
            ${opengl_lib})

    add_executable(pong_opengl main.cpp)
    target_link_libraries(pong_opengl ${framework} glfw glew)
endif(APPLE)