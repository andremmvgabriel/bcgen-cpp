#include <SignedVar.hpp>

gabe::circuits::generator::SignedVar::SignedVar() {}

gabe::circuits::generator::SignedVar::SignedVar(uint64_t number_wires) : number_wires(number_wires) { wires.resize(number_wires); }
