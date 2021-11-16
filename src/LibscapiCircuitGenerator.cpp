#include <LibscapiCircuitGenerator.hpp>

gabe::circuits::generator::LibscapiCircuitGenerator::LibscapiCircuitGenerator() {}

gabe::circuits::generator::LibscapiCircuitGenerator::LibscapiCircuitGenerator(const std::string &circuit_name, const std::vector<uint64_t>& wires_per_input_party, const std::vector<uint64_t>& wires_per_output_party, const std::string &circuits_directory) : CircuitGenerator("libscapi_" + circuit_name, wires_per_input_party, wires_per_output_party, circuits_directory) {
    // Assigns the correct gates names
    _gates_map["xor"] = "0110";
    _gates_map["and"] = "0001";
    _gates_map["inv"] = "10";
    _gates_map["or"] = "0111";
}

void gabe::circuits::generator::LibscapiCircuitGenerator::_write_header() {
    _write_header_info();
    _write_header_inputs();
    _write_header_outputs();
}

void gabe::circuits::generator::LibscapiCircuitGenerator::_write_header_info() {
    uint64_t number_gates = _counter_and_gates + _counter_xor_gates + _counter_inv_gates + _counter_or_gates;

    std::string line = std::to_string(number_gates) + " " + std::to_string(_wires_per_input_party.size()) + "\n";

    _circuit.write( line.c_str(), line.size() );

    std::string empty_line = "\n";
    _circuit.write(empty_line.c_str(), empty_line.size());
}

void gabe::circuits::generator::LibscapiCircuitGenerator::_write_header_inputs() {
    uint64_t inputs_written = 0x00;

    for (int i = 0; i < _wires_per_input_party.size(); i++) {
        uint64_t party_size = _wires_per_input_party.at(i);

        std::string line = std::to_string(i + 1) + " " + std::to_string(party_size) + "\n";

        for (uint64_t j = 0; j < party_size; j++)
            line += std::to_string(inputs_written + j) + "\n";
        
        line += "\n";
        _circuit.write(line.c_str(), line.size());
        
        inputs_written += party_size;
    }
}

void gabe::circuits::generator::LibscapiCircuitGenerator::_write_header_outputs() {
    uint64_t outputs_to_write = 0x00;
    for (auto & amount : _wires_per_output_party)
        outputs_to_write += amount;
    
    if (_wires_per_output_party.size() == 1) {
        uint64_t party_size = _wires_per_output_party.at(0);

        std::string line = std::to_string(party_size) + "\n";

        for (uint64_t j = 0; j < party_size; j++)
            line += std::to_string(_counter_wires - outputs_to_write + j) + "\n";
        
        line += "\n";
        _circuit.write(line.c_str(), line.size());
    }
    else {
        uint64_t outputs_written = 0x00;

        for (int i = 0; i < _wires_per_output_party.size(); i++) {
            uint64_t party_size = _wires_per_output_party.at(i);

            std::string line = std::to_string(i + 1) + " " + std::to_string(party_size) + "\n";

            for (uint64_t j = 0; j < party_size; j++)
                line += std::to_string(_counter_wires - outputs_to_write + outputs_written + j) + "\n";
            
            line += "\n";
            _circuit.write(line.c_str(), line.size());
            
            outputs_written += party_size;
        }
    }
}
