#include <bcgen/variables.hpp>

// ****
// Variable class
// ****

gabe::bcgen::Variable::Variable() {}

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

// ****
// SignedVar class
// ****

gabe::bcgen::SignedVar::SignedVar() {}

gabe::bcgen::SignedVar::SignedVar(uint64_t number_wires) : Variable(number_wires) {}

// ****
// UnsignedVar class
// ****

gabe::bcgen::UnsignedVar::UnsignedVar() {}

gabe::bcgen::UnsignedVar::UnsignedVar(uint64_t number_wires) : Variable(number_wires) {}
