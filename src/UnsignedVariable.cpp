#include <UnsignedVariable.hpp>

gabe::circuits::generator::UnsignedVariable::UnsignedVariable() {}

gabe::circuits::generator::UnsignedVariable::UnsignedVariable(uint64_t number_wires) : number_wires(number_wires) { wires.resize(number_wires); }
