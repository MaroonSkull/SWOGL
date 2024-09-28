cmake_minimum_required(VERSION 3.14)
project(swogl-add LANGUAGES CXX)

include("../../cmake/utils.cmake")
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

if(is_top_level)
    find_package(swogl REQUIRED)
endif()

set(sources main.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

add_executable(swogl-add)
target_sources(swogl-add PRIVATE ${sources})
target_link_libraries(swogl-add PRIVATE swogl::swogl)

if(NOT is_top_level)
    win_copy_deps_to_target_dir(swogl-add swogl::swogl)
endif()
