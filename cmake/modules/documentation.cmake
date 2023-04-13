if(BCGEN_DOCS)
    # Writes the Doxygen configuration file
    configure_file(
        ${PROJECT_SOURCE_DIR}/cmake/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY
    )

    # Find Doxygen & dot commands
    find_program(DoxCommand doxygen REQUIRED)
    find_program(DotCommand dot REQUIRED) # Graphviz

    # Doxygen-Awesome project for modern style
    FetchContent_Declare(
        doxygenawesome
        GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
        GIT_TAG v2.2.0
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(doxygenawesome)

    # Generate doxygen documentation
    execute_process(COMMAND ${DoxCommand} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
endif(BCGEN_DOCS)
