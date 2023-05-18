# Quality of Life function to create new project example
# > add_example (function name)
# > EXAMPLE_NAME (string -> name of the example)
function(add_example)
    # Arguments parsing
    set(options "")
    set(oneValueArgs EXAMPLE_NAME)
    set(multiValueArgs "")
    cmake_parse_arguments(
        ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    # Creates the example target
    add_executable(${ARGS_EXAMPLE_NAME} ${ARGS_EXAMPLE_NAME}.cpp)

    # Linking bcgen to the example target
    target_link_libraries(
        ${ARGS_EXAMPLE_NAME} PRIVATE bcgen::bcgen
    )
endfunction(add_example)

# Checks if the Examples flag was used
if(BCGEN_EXAMPLES)
    # Creates all the examples
    add_example(EXAMPLE_NAME addition64)
    add_example(EXAMPLE_NAME subtract64)
    add_example(EXAMPLE_NAME negation64)
    add_example(EXAMPLE_NAME signed_multiplication64_128)
    add_example(EXAMPLE_NAME unsigned_multiplication64_128)
    add_example(EXAMPLE_NAME signed_division64)
    add_example(EXAMPLE_NAME unsigned_division64)
    add_example(EXAMPLE_NAME zero_equality64)
    add_example(EXAMPLE_NAME aes128_ecb_encryption)
    add_example(EXAMPLE_NAME aes128_ecb_decryption)
endif(BCGEN_EXAMPLES)
