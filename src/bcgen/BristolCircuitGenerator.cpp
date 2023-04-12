#include <bcgen/BristolCircuitGenerator.hpp>
#include <fmt/format.h>

gabe::bcgen::BristolCircuitGenerator::BristolCircuitGenerator(const std::string &circuit_name) : CircuitGenerator("bristol_" + circuit_name) {
    _gates_map = { {"xor", "XOR"}, {"inv", "INV"}, {"and", "AND"}, {"or", "OR"} };
}

gabe::bcgen::BristolCircuitGenerator::BristolCircuitGenerator(const std::string &circuit_name, const std::string &circuits_directory) : CircuitGenerator("bristol_" + circuit_name, circuits_directory) {
    _gates_map = { {"xor", "XOR"}, {"inv", "INV"}, {"and", "AND"}, {"or", "OR"} };
}

void gabe::bcgen::BristolCircuitGenerator::_write_header(std::ofstream& file) {
    // Header construction
    const std::string header = fmt::format("{} {}\n", _counter_gates, _counter_wires);
    const std::string inputs = fmt::format("{} {}\n", _input_parties.size(), fmt::join(_input_parties, " "));
    const std::string outputs = fmt::format("{} {}\n", _output_parties.size(), fmt::join(_output_parties, " "));
    const std::string linebrk = "\n";

    // Header writing
    file.write(header.c_str(), header.size());
    file.write(inputs.c_str(), inputs.size());
    file.write(outputs.c_str(), outputs.size());
    file.write(linebrk.c_str(), linebrk.size());
}
