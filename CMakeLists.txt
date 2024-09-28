cmake_minimum_required(VERSION 3.14)
project(swogl
    VERSION 0.0.0
    DESCRIPTION "Stupid C++ wrapper for OpenGL"
    LANGUAGES CXX)

#----------------------------------------------------------------------------------------------------------------------
# general settings and options
#----------------------------------------------------------------------------------------------------------------------

include(cmake/utils.cmake)
include(GNUInstallDirs)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

# SWOGL_SHARED_LIBS option (undefined by default) can be used to force shared/static build
option(SWOGL_BUILD_EXAMPLES "Build swogl examples" OFF)
option(SWOGL_BUILD_DOCS "Build swogl documentation" OFF)
option(SWOGL_INSTALL "Generate target for installing swogl" ${is_top_level})
set_if_undefined(SWOGL_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/swogl" CACHE STRING
    "Install path for swogl package-related CMake files")

if(DEFINED SWOGL_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ${SWOGL_SHARED_LIBS})
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set_if_undefined(CMAKE_CXX_VISIBILITY_PRESET hidden)
set_if_undefined(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

add_library(swogl) # initialized below
add_library(swogl::swogl ALIAS swogl)

#----------------------------------------------------------------------------------------------------------------------
# swogl dependencies
#----------------------------------------------------------------------------------------------------------------------

# Search for your dependencies here

#----------------------------------------------------------------------------------------------------------------------
# swogl sources
#----------------------------------------------------------------------------------------------------------------------

include(GenerateExportHeader)
set(export_file_name "export_shared.h")

if(NOT BUILD_SHARED_LIBS)
    set(export_file_name "export_static.h")
endif()

generate_export_header(swogl EXPORT_FILE_NAME include/swogl/${export_file_name})

set(sources
    include/swogl/export.h
    include/swogl/swogl.h
    src/swogl.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

#----------------------------------------------------------------------------------------------------------------------
# swogl target
#----------------------------------------------------------------------------------------------------------------------

include(CMakePackageConfigHelpers)

target_sources(swogl PRIVATE ${sources})
target_compile_definitions(swogl PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:SWOGL_STATIC_DEFINE>")

target_include_directories(swogl
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

set_target_properties(swogl PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION})

if(SWOGL_INSTALL AND NOT CMAKE_SKIP_INSTALL_RULES)
    configure_package_config_file(cmake/swogl-config.cmake.in swogl-config.cmake
        INSTALL_DESTINATION "${SWOGL_INSTALL_CMAKEDIR}")

    write_basic_package_version_file(swogl-config-version.cmake
        COMPATIBILITY SameMajorVersion)

    install(TARGETS swogl EXPORT swogl_export
        RUNTIME COMPONENT swogl
        LIBRARY COMPONENT swogl NAMELINK_COMPONENT swogl-dev
        ARCHIVE COMPONENT swogl-dev
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
    install(DIRECTORY include/
        TYPE INCLUDE
        COMPONENT swogl-dev)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/include/swogl/${export_file_name}"
        COMPONENT swogl-dev
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/swogl")

    set(targets_file "swogl-shared-targets.cmake")

    if(NOT BUILD_SHARED_LIBS)
        set(targets_file "swogl-static-targets.cmake")
    endif()

    install(EXPORT swogl_export
        COMPONENT swogl-dev
        FILE "${targets_file}"
        DESTINATION "${SWOGL_INSTALL_CMAKEDIR}"
        NAMESPACE swogl::)

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/swogl-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/swogl-config-version.cmake"
        COMPONENT swogl-dev
        DESTINATION "${SWOGL_INSTALL_CMAKEDIR}")

    if(MSVC)
        set(pdb_file "")
        set(pdb_file_destination "")

        if(BUILD_SHARED_LIBS)
            set(pdb_file "$<TARGET_PDB_FILE:swogl>")
            set(pdb_file_destination "${CMAKE_INSTALL_BINDIR}")
        else()
            # TARGET_PDB_FILE does not work for pdb file generated for static library build, determining it manually
            set(pdb_file "$<TARGET_FILE_DIR:swogl>/$<TARGET_FILE_PREFIX:swogl>$<TARGET_FILE_BASE_NAME:swogl>.pdb")
            set(pdb_file_destination "${CMAKE_INSTALL_LIBDIR}")
        endif()

        install(FILES "${pdb_file}"
            COMPONENT swogl-dev
            CONFIGURATIONS Debug RelWithDebInfo
            DESTINATION "${pdb_file_destination}"
            OPTIONAL)
    endif()
endif()

#----------------------------------------------------------------------------------------------------------------------
# other targets
#----------------------------------------------------------------------------------------------------------------------

if(SWOGL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(SWOGL_BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    doxygen_add_docs(docs include)
endif()
