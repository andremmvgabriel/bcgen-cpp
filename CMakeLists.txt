cmake_minimum_required(VERSION 3.12)

project(BCGen
    VERSION 0.1.0
    DESCRIPTION "BCGen stands for Boolean Circuit Generator, and it is a library developed to generate boolean circuits in text files that can be used by cryptographic libraries for multiparty computations."
    LANGUAGES CXX
)

###
# Includes
###
include(FetchContent)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

###
# Project Options
###
option(BCGEN_OR_GATES "Allow generated circuits with OR gates" ON)
option(BCGEN_OPTIMIZE "Build bcgen in optimized mode." OFF)
option(BCGEN_DOCS "Generate bcgen project Doxygen documentation" OFF)
option(BCGEN_TESTS "Build bcgen tests." OFF)
option(BCGEN_EXAMPLES "Generate bcgen examples." OFF)

###
# Project Includes
###
include(cmake/modules/versioning.cmake)
include(cmake/modules/documentation.cmake)

###
# Project Dependencies
###

set(FETCHCONTENT_QUIET FALSE)

find_package(fmt QUIET)
if (NOT fmt_FOUND)
    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(fmt)
    set_target_properties(fmt PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()

###
# Create Library
###
set(bcgen_sources
    ${CMAKE_BINARY_DIR}/bcgen/version.cpp
    src/bcgen/Variables.cpp
    src/bcgen/CircuitGenerator.cpp
    src/bcgen/BristolCircuitGenerator.cpp
    src/bcgen/LibscapiCircuitGenerator.cpp
    src/bcgen/CircuitTester.cpp
    src/bcgen/BristolCircuitTester.cpp
    src/bcgen/LibscapiCircuitTester.cpp
)
add_library(bcgen ${bcgen_sources})
add_library(bcgen::bcgen ALIAS bcgen)
target_include_directories(bcgen PUBLIC include build/_deps)
target_link_libraries(bcgen PUBLIC fmt::fmt)
target_compile_definitions(bcgen PUBLIC
    BCGEN_OR_GATES=$<IF:$<BOOL:${BCGEN_OR_GATES}>,1,0>
    BCGEN_OPTIMIZE=$<IF:$<BOOL:${BCGEN_OPTIMIZE}>,1,0>
)

###
# Tests & Examples
###
add_subdirectory( tests )
add_subdirectory( examples )
