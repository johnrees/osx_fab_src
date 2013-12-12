CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

find_package(Boost REQUIRED COMPONENTS thread system)
include_directories(${Boost_INCLUDE_DIR})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "-Wall -O3")
#set(CMAKE_CXX_FLAGS "-Wall -g")

ADD_LIBRARY(MathTree
            parser.cpp math_tree.cpp region.cpp
            numeric_nodes.cpp logic_nodes.cpp
            translator_nodes.cpp color_nodes.cpp
            fabvars.cpp node.cpp opcodes.cpp geometry.cpp
            progress_bar.cpp task_buffer.cpp solver.cpp)

ADD_EXECUTABLE(math_png math_png.cpp imagesolver.cpp)
ADD_EXECUTABLE(math_ray math_ray.cpp raycaster.cpp)
ADD_EXECUTABLE(math_dot math_dot.cpp)
ADD_EXECUTABLE(math_stl math_stl.cpp trisolver.cpp)
ADD_EXECUTABLE(math_svg math_svg.cpp edgesolver.cpp)
ADD_EXECUTABLE(math_stats math_stats.cpp volsolver.cpp)

SET(LIBRARIES
    MathTree ${PNG_LIBRARY}
    ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

# Ubuntu 13.10 requires us to link against pthread
EXECUTE_PROCESS(COMMAND sh -c "which lsb_release >/dev/null && lsb_release -r | grep 13.10" OUTPUT_VARIABLE ubuntu13_10)
if (NOT "${ubuntu13_10}" STREQUAL "")
    SET(LIBRARIES ${LIBRARIES} pthread)
    MESSAGE(STATUS "Ubuntu 13.10 detected: Adding pthread to linked libraries")
endif()

TARGET_LINK_LIBRARIES (math_png ${LIBRARIES})
TARGET_LINK_LIBRARIES (math_ray ${LIBRARIES})
TARGET_LINK_LIBRARIES (math_dot ${LIBRARIES})
TARGET_LINK_LIBRARIES (math_stl ${LIBRARIES})
TARGET_LINK_LIBRARIES (math_svg ${LIBRARIES})
TARGET_LINK_LIBRARIES (math_stats ${LIBRARIES})

if( ${CMAKE_PROJECT_NAME} MATCHES fabmod )
  install(TARGETS math_png DESTINATION ${PROJECT_SOURCE_DIR}/../bin)
  install(TARGETS math_ray DESTINATION ${PROJECT_SOURCE_DIR}/../bin)
  install(TARGETS math_dot DESTINATION ${PROJECT_SOURCE_DIR}/../bin)
  install(TARGETS math_stl DESTINATION ${PROJECT_SOURCE_DIR}/../bin)
  install(TARGETS math_svg DESTINATION ${PROJECT_SOURCE_DIR}/../bin)
  install(TARGETS math_stats DESTINATION ${PROJECT_SOURCE_DIR}/../bin)
endif( ${CMAKE_PROJECT_NAME} MATCHES fabmod )

set(SOLVER_EXECUTABLES math_png math_dot math_stl math_svg
    CACHE STRING "Revised solver executables")
