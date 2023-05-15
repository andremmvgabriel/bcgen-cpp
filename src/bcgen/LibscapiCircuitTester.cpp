#include <bcgen/CircuitTester.hpp>
#include <string>
#include <vector>
#include <cstdint>

gabe::bcgen::LibscapiCircuitTester::LibscapiCircuitTester() : CircuitTester() {
    _gates_operations = {
        { "0110", [&](const std::vector<std::string>& gate){ return _wires[std::stoi(gate[2])] ^ _wires[std::stoi(gate[3])]; } },
        { "01",   [&](const std::vector<std::string>& gate){ return !_wires[std::stoi(gate[2])]; } },
        { "0001", [&](const std::vector<std::string>& gate){ return _wires[std::stoi(gate[2])] & _wires[std::stoi(gate[3])]; } },
        { "0111", [&](const std::vector<std::string>& gate){ return _wires[std::stoi(gate[2])] | _wires[std::stoi(gate[3])]; } }
    };
}

void gabe::bcgen::LibscapiCircuitTester::_read_header() {
    std::string line;
    std::vector<std::string> line_parts;
    
    // Reads the first line
    // > Should contain the total number of gates followed by the total number of input parties
    std::getline(_circuit, line);
    line_parts = _split_str(line, " ");
    _counter_gates = std::stoul(line_parts[0]);
    int n_input_parties = std::stoi(line_parts[1]);

    // Ignores the next line (empty line)
    std::getline(_circuit, line);

    // Reads all the inputs lines
    // > First line is the party number and followed by its amount
    // > Next lines should contain the wire labels of all the party input wires
    // > Next line is empty -> ignore.
    for (int i = 0; i < n_input_parties; i++) {
        // Reads the party number and its size
        std::getline(_circuit, line);
        line_parts = _split_str(line, " ");
        int n_wires = std::stoi(line_parts[1]);
        _input_parties.push_back(n_wires);

        // Reads the party wire labels
        std::vector<uint64_t> cur_party_wires;
        for (int i = 0; i < n_wires; i++) {
            std::getline(_circuit, line);
            cur_party_wires.push_back(std::stoi(line));
        }
        _input_parties_wires.push_back(cur_party_wires);

        // Ignores the next line (empty line)
        std::getline(_circuit, line);
    }

    // Reads all the output lines
    // > First line is the party number and followed by its amount
    //   > Since we don't know how many output parties there are, we don't know when to stop searching. As so, in this step, this
    //    first line can be already the beginning of the circuit. If so, stop searching.
    // > Next lines should contain the wire labels of all the party output wires
    // > Next line is empty -> ignore.
    for (int i = 0; i < n_input_parties; i++) {
        // Reads the party number and its size
        std::getline(_circuit, line);
        line_parts = _split_str(line, " ");

        // Check if the beginning of the circuit was found instead
        if (line_parts.size() <= 2) { break; }

        int n_wires = std::stoi(line_parts[1]);
        _input_parties.push_back(n_wires);

        // Reads the party wire labels
        std::vector<uint64_t> cur_party_wires;
        for (int i = 0; i < n_wires; i++) {
            std::getline(_circuit, line);
            uint64_t wire_label = std::stoi(line);
            cur_party_wires.push_back(wire_label);
            if (wire_label > _counter_wires) {
                _counter_wires = wire_label;
            }
        }
        _input_parties_wires.push_back(cur_party_wires);

        // Ignores the next line (empty line)
        std::getline(_circuit, line);

        // Updates the current read index (start of the circuit)
        _circuit_start_pos = _circuit.tellg();
    }

    // Makes sure the reading pointer is back to the beginning of the circuit
    _circuit.seekg(_circuit_start_pos);

    // 
    _wires = std::vector<uint8_t>(_counter_wires);

    // Prints circuit information to the user
    _print_circuit_info();
}
