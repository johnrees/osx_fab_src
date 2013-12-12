cmake_minimum_required(VERSION 2.6)
project(fabmod)

set(CMAKE_BUILD_TYPE Release)

add_subdirectory(core)
add_subdirectory(solver)
add_subdirectory(scripts)
add_subdirectory(guis)
add_subdirectory(py)
