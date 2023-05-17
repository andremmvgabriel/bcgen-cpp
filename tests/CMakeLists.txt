# Quality of Life function to create new project test
# > add_unit_test (function name)
# > TEST_SUIT (string -> name of the suit where the test will be placed)
# > TEST_NAME (string -> name of the test)
function(add_unit_test)
    # Arguments parsing
    set(options "")
    set(oneValueArgs TEST_SUIT TEST_NAME)
    set(multiValueArgs "")
    cmake_parse_arguments(
        ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    # Creates the test target
    # > UUT: Unit Under Test
    # > A unit test has to have both its target and cpp file with an UUT_ prefix, and their whole name should match.
    add_executable(UUT_${ARGS_TEST_NAME} UUT_${ARGS_TEST_NAME}.cpp)

    # Linking bcgen and Catch2 testing framework to the test target
    target_link_libraries(
        UUT_${ARGS_TEST_NAME} PRIVATE bcgen::bcgen Catch2::Catch2WithMain
    )

    # CTest require this command to be executed to add the target to their tests control
    add_test(NAME UUT_${ARGS_TEST_NAME} COMMAND UUT_${ARGS_TEST_NAME})

    # Adds the current test to the test suit list
    # > This list will be used to set a global target for the tests
    set(TEST_SUIT_${ARGS_TEST_SUIT}
        ${TEST_SUIT_${ARGS_TEST_SUIT}} UUT_${ARGS_TEST_NAME}
        PARENT_SCOPE
    )

    # Adds the test suit to the test suit list
    if(NOT "${ARGS_TEST_SUIT}" IN_LIST TEST_SUITS)
        set(TEST_SUITS
            ${ARGS_TEST_SUITS} ${ARGS_TEST_SUIT}
            PARENT_SCOPE
        )
    endif()
endfunction(add_unit_test)

# Quality of Life function to create the test suits targets of the project
# > create_test_suits_targets (function name)
function(create_test_suits_targets)
    # Iterates all the defined test suits, if any
    foreach(suit ${TEST_SUITS})
        # Creates a target for the test suit
        add_custom_target("${suit}")

        # Adds the list of test targets of that test suit into the new target
        add_dependencies("${suit}" ${TEST_SUIT_${suit}})
    endforeach(suit ${TEST_SUITS})
endfunction(create_test_suits_targets)



# Checks if the Test flag was used
if(BCGEN_TESTS)
    # CTest tests enable (if ommitted, CTests do not recognize any test)
    enable_testing()

    # All the tests need Catch2 testing framework
    find_package(Catch2 QUIET)

    if(NOT Catch2_FOUND)
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG        v3.3.2
            GIT_PROGRESS TRUE
        )
        FetchContent_MakeAvailable(Catch2)
        set_target_properties(Catch2 PROPERTIES EXCLUDE_FROM_ALL TRUE)
    endif()

    # Creates all the tests
    add_unit_test(TEST_SUIT all_tests TEST_NAME variables)
    add_unit_test(TEST_SUIT all_tests TEST_NAME CircuitGenerator)

    # Makes sure that all the test suits have their own callable target from cmake --build command
    create_test_suits_targets()
endif(BCGEN_TESTS)
