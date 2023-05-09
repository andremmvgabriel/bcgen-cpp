#include <bcgen/CircuitGenerator.hpp>

gabe::bcgen::Variable::Variable(uint64_t number_wires) {
    _wires.resize(number_wires);
}

gabe::bcgen::Wire& gabe::bcgen::Variable::operator[](uint64_t index) {
    return _wires[index];
}

const gabe::bcgen::Wire& gabe::bcgen::Variable::operator[](uint64_t index) const {
    return _wires[index];
}

uint64_t gabe::bcgen::Variable::size() const {
    return _wires.size();
}
