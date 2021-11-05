#include <UnsignedVar.hpp>

gabe::circuits::generator::UnsignedVar::UnsignedVar() {}

gabe::circuits::generator::UnsignedVar::UnsignedVar(uint64_t number_wires) : number_wires(number_wires) { wires.resize(number_wires); }
