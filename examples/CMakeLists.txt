add_executable( XOR_Circuit XOR.cpp )
target_link_libraries( XOR_Circuit BooleanCircuitGenerator )

# ======

add_executable( AND_Circuit AND.cpp )
target_link_libraries( AND_Circuit BooleanCircuitGenerator )

# ======

add_executable( INV_Circuit INV.cpp )
target_link_libraries( INV_Circuit BooleanCircuitGenerator )
