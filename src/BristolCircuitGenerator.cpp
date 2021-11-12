#include <BristolCircuitGenerator.hpp>

gabe::circuits::generator::BristolCircuitGenerator::BristolCircuitGenerator() {}

gabe::circuits::generator::BristolCircuitGenerator::BristolCircuitGenerator(const std::string &circuit_name, const std::vector<uint64_t>& wires_per_input_party, const std::vector<uint64_t>& wires_per_output_party, const std::string &circuits_directory) : CircuitGenerator("bristol_" + circuit_name, wires_per_input_party, wires_per_output_party, circuits_directory) {
    // Assigns the correct gates names
    _gates_map["xor"] = "XOR";
    _gates_map["and"] = "AND";
    _gates_map["inv"] = "INV";
    _gates_map["or"] = "OR";
}
