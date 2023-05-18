# Boolean Circuit Generator (BGGen)

## Brief

## Platforms

This project was developed in Linux, thus only linux system can currently use it.

## Documentation

All the code is documented. Proper documentation is present in the header files, and proper comments are written along the code in the cpp files.

The documentation present in the header files can also be generated into an HTML format (doxygen). To generate it, please build the project using the docs flags, as follows:

`cmake -B build -S . -DBCGEN_DOCS=ON && cmake --build build`

## Limitations

Currently, the project has the following limitations:
* Only supports integer variables. Decimal values are not supported (yet).
* The functions do not allow variables with arbitrary sizes between the variables. In most functions, all the inserted variables must have the same size, including the output variables.

## Tests

The project is setup with several unit tests that can be executed to make sure everything is working properly. To compile them, please build the project using the tests flags, as follows:

`cmake -B build -S . -DBCGEN_TESTS=ON && cmake --build build`

## Examples

The project is setup with several examples that can be executed to generate specific circuits that might help an user. To compile them, please build the project using the examples flags, as follows:

`cmake -B build -S . -DBCGEN_EXAMPLES=ON && cmake --build build`

## TODO

1. Make use of BCGEN_OPTIMIZE CMake flag.
   1. Adapt methods to accept variables with arbitrary sizes;
   2. Keep track of the created gates so there isn't duplicated gates in the circuit.
2. Implement operations on other types (currently only integers are supported).
