####

add_executable( UUT_CircuitGenerator
    UUT_CircuitGenerator.cpp
)
target_link_libraries( UUT_CircuitGenerator
    BooleanCircuitGenerator
    -lboost_unit_test_framework
) 
set_target_properties( UUT_CircuitGenerator
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin/unit_tests
)

####
