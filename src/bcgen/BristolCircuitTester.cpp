#include <bcgen/CircuitTester.hpp>
#include <string>
#include <vector>
#include <cstdint>

gabe::bcgen::BristolCircuitTester::BristolCircuitTester() : CircuitTester() {
    _gates_operations = {
        { "XOR", [&](const std::vector<std::string>& gate){ return _wires[std::stoi(gate[2])] ^ _wires[std::stoi(gate[3])]; } },
        { "INV", [&](const std::vector<std::string>& gate){ return !_wires[std::stoi(gate[2])]; } },
        { "AND", [&](const std::vector<std::string>& gate){ return _wires[std::stoi(gate[2])] & _wires[std::stoi(gate[3])]; } },
        { "OR",  [&](const std::vector<std::string>& gate){ return _wires[std::stoi(gate[2])] | _wires[std::stoi(gate[3])]; } }
    };
}

void gabe::bcgen::BristolCircuitTester::_read_header() {
    std::string line;
    std::vector<std::string> line_parts;
    
    // Reads the first line
    // > Should contain the total number of gates followed by the total number of wires
    std::getline(_circuit, line);
    line_parts = _split_str(line, " ");
    _counter_gates = std::stoul(line_parts[0]);
    _counter_wires = std::stoul(line_parts[1]);

    // Reads the second line
    // > Should contain the amount of input parties followed by their sizes
    std::getline(_circuit, line);
    line_parts = _split_str(line, " ");
    int input_wires_counter = 0;
    int n_input_parties = std::stoi(line_parts[0]);
    for (int i = 0; i < n_input_parties; i++) {
        int cur_party_size = std::stoi(line_parts[i+1]);
        _input_parties.push_back(cur_party_size);
        std::vector<uint64_t> cur_party_wires;
        for (int j = 0; j < cur_party_size; j++) {
            cur_party_wires.push_back(input_wires_counter++);
        }
        _input_parties_wires.push_back(cur_party_wires);
    }

    // Reads the third line
    // > Should contain the amount of output parties followed by their sizes
    std::getline(_circuit, line);
    line_parts = _split_str(line, " ");
    int output_wires_counter = 0;
    int total_output_wires = 0;
    int n_output_parties = std::stoi(line_parts[0]);
    for (int i = 0; i < n_output_parties; i++) {
        int cur_party_size = std::stoi(line_parts[i+1]);
        _output_parties.push_back(cur_party_size);
        total_output_wires += cur_party_size;
    }
    for (int i = 0; i < n_output_parties; i++) {
        std::vector<uint64_t> cur_party_wires;
        for (int j = 0; j < _output_parties[i]; j++) {
            cur_party_wires.push_back(_counter_wires - total_output_wires + output_wires_counter);
            output_wires_counter++;
        }
        _output_parties_wires.push_back(cur_party_wires);
    }

    // Ignores the next line (empty line)
    std::getline(_circuit, line);

    // Saves the current read index (start of the circuit)
    _circuit_start_pos = _circuit.tellg();

    // 
    _wires = std::vector<uint8_t>(_counter_wires);

    // Prints circuit information to the user
    _print_circuit_info();
}
