cmake_minimum_required(VERSION 3.12)

project(BooleanCircuitGenerator
    VERSION 0.1.0
    DESCRIPTION "TBD"
    LANGUAGES CXX
)



###
# Includes
###
include(FetchContent)



# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find_program(CLANG_TIDY_EXE clang-tidy)
# if(CLANG_TIDY_EXE)
#     set(CMAKE_C_CLANG_TIDY "${CLANG_TIDY_EXE}")
#     set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
# endif()


# Changes the binary, static lib, and libraries default paths of the CMake build, respectively
# set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
# set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
# set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)



###
# Versioning
###
configure_file(
    cmake/version.cpp.in
    ${CMAKE_BINARY_DIR}/bcgen/version.cpp
    @ONLY
)



###
# Project Options
###
option(OR_GATES "Allow generated circuits with OR gates" 1)



###
# Project Dependencies
###
set(FETCHCONTENT_QUIET FALSE)

find_package(spdlog QUIET)
if (NOT spdlog_FOUND)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.11.0
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(spdlog)
    set_target_properties(spdlog PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()



###
# Create Library
###
set(bcgen_sources
    ${CMAKE_BINARY_DIR}/bcgen/version.cpp
    src/bcgen/variables.cpp
    src/bcgen/CircuitGenerator.cpp
    src/bcgen/BristolCircuitGenerator.cpp
    src/bcgen/LibscapiCircuitGenerator.cpp
)
add_library(bcgen ${bcgen_sources})
add_library(bcgen::bcgen ALIAS bcgen)
target_include_directories(bcgen PUBLIC include build/_deps)
target_link_libraries(bcgen PUBLIC spdlog::spdlog_header_only)
target_compile_definitions(bcgen PUBLIC ALLOW_OR_GATES=${OR_GATES})



# add_subdirectory( tests )
# add_subdirectory( examples )
