cmake_minimum_required(VERSION 2.6)

set(PROGRAMS vol_gif gif_png gif_stl stl_info stl_png stl_path
             png_size png_scale png_distances png_offset png_grb png_drl
             png_path png_halftone svg_path
             path_eps path_dxf path_png path_rml path_sbp path_g path_camm
             path_epi path_uni path_oms path_ord
             path_join path_array path_info path_time
    CACHE STRING "Core program list")

find_package(PNG REQUIRED)
find_package(GIF REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
include_directories(${GIF_INCLUDE_DIR})

#set(CMAKE_C_FLAGS "-Wall -g")
set(CMAKE_C_FLAGS "-Wall -O3")

add_library(fabcore STATIC fab.c)

foreach(program ${PROGRAMS})
  add_executable(${program} ${program}.c)
  target_link_libraries(${program} fabcore ${PNG_LIBRARY} ${GIF_LIBRARY} m)
endforeach(program)

if( ${CMAKE_PROJECT_NAME} MATCHES fabmod )
  install(TARGETS ${PROGRAMS} DESTINATION ${PROJECT_SOURCE_DIR}/../bin)
endif( ${CMAKE_PROJECT_NAME} MATCHES fabmod )
