#include <LibscapiCircuitGenerator.hpp>

gabe::circuits::generator::LibscapiCircuitGenerator::LibscapiCircuitGenerator() {}

gabe::circuits::generator::LibscapiCircuitGenerator::LibscapiCircuitGenerator(const std::string &circuit_name, const std::vector<uint64_t>& wires_per_input_party, const std::vector<uint64_t>& wires_per_output_party, const std::string &circuits_directory) : CircuitGenerator("libscapi_" + circuit_name, wires_per_input_party, wires_per_output_party, circuits_directory) {
    // Assigns the correct gates names
    _gates_map["xor"] = "0110";
    _gates_map["and"] = "0001";
    _gates_map["inv"] = "10";
    _gates_map["or"] = "0111";
}
