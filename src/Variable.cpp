#include <Variable.hpp>

//gabe::circuits::generator::Wire gabe::circuits::generator::Variable::operator[](uint32_t index) {
//    return wires[index];
//}

gabe::circuits::generator::Variable::Variable() {}

gabe::circuits::generator::Variable::Variable(uint64_t number_wires) {
    wires.resize(number_wires);
}

gabe::circuits::generator::Wire& gabe::circuits::generator::Variable::operator[](uint64_t index) {
    return wires[index];
}

uint64_t gabe::circuits::generator::Variable::size() {
    return wires.size();
}