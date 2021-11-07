cmake_minimum_required(VERSION 3.10)

project(BooleanCircuitGenerator)

#find_package( Boost REQUIRED COMPONENTS unit_test_framework )

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Changes the binary, static lib, and libraries default paths of the CMake build, respectively
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)

INCLUDE_DIRECTORIES ( include )


set( src_generator_lib
    src/Wire.cpp
    src/Variable.cpp
    src/SignedVar.cpp
    src/UnsignedVar.cpp
    src/CircuitGenerator.cpp
)
add_library( BooleanCircuitGenerator STATIC ${src_generator_lib} )

add_subdirectory( tests )
add_subdirectory( examples )