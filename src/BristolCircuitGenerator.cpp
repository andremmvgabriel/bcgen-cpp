#include <BristolCircuitGenerator.hpp>

gabe::circuits::generator::BristolCircuitGenerator::BristolCircuitGenerator() {}

gabe::circuits::generator::BristolCircuitGenerator::BristolCircuitGenerator(const std::string &circuit_name, const std::vector<uint64_t>& wires_per_input_party, const std::vector<uint64_t>& wires_per_output_party, const std::string &circuits_directory) : CircuitGenerator("bristol_" + circuit_name, wires_per_input_party, wires_per_output_party, circuits_directory) {
    // Assigns the correct gates names
    _gates_map["xor"] = "XOR";
    _gates_map["and"] = "AND";
    _gates_map["inv"] = "INV";
    _gates_map["or"] = "OR";
}

void gabe::circuits::generator::BristolCircuitGenerator::_write_header() {
    _write_header_info();
    _write_header_inputs();
    _write_header_outputs();

    std::string empty_line = "\n";
    _circuit.write(empty_line.c_str(), empty_line.size());
}

void gabe::circuits::generator::BristolCircuitGenerator::_write_header_info() {
    uint64_t number_gates = _counter_and_gates + _counter_xor_gates + _counter_inv_gates + _counter_or_gates;

    std::string line = std::to_string(number_gates) + " " + std::to_string(_counter_wires) + "\n";

    _circuit.write( line.c_str(), line.size() );
}

void gabe::circuits::generator::BristolCircuitGenerator::_write_header_inputs() {
    std::string line = std::to_string(_wires_per_input_party.size());

    for (auto & amount : _wires_per_input_party)
        line += " " + std::to_string(amount);
    
    line += "\n";

    _circuit.write( line.c_str(), line.size() );
}

void gabe::circuits::generator::BristolCircuitGenerator::_write_header_outputs() {
    std::string line = std::to_string(_wires_per_output_party.size());

    for (auto & amount : _wires_per_output_party)
        line += " " + std::to_string(amount);
    
    line += "\n";

    _circuit.write( line.c_str(), line.size() );
}
