cmake_minimum_required(VERSION 3.25)
project(AStar)

set(CMAKE_CXX_STANDARD 17)

add_library(AStar SHARED AStar.h AStar.cpp)

# 在 Windows 环境下，根据不同的配置链接运行时库到 DLL 中
if (WIN32)
    target_compile_options(AStar PRIVATE
            "$<$<CONFIG:Debug>:/MTd>"
            "$<$<CONFIG:Release>:/MT>"
    )
endif()
