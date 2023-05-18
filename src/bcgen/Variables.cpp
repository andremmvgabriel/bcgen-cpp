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

void gabe::bcgen::Variable::operator=(const Variable& other) {
    // TODO: Improve this...
    if (_wires.size() != other.size()) {
        // Error message
        const std::string error_msg = "Cannot use equal operation on variables with different sizes.";
        
        // Raises the exception
        throw std::runtime_error(error_msg);
    }

    for (int i = 0; i < _wires.size(); i++) {
        _wires[i].label = other[i].label;
    }
}

uint64_t gabe::bcgen::Variable::size() const {
    return _wires.size();
}
