#include <CircuitGenerator.hpp>

gabe::circuits::generator::CircuitGenerator::CircuitGenerator() {}

gabe::circuits::generator::CircuitGenerator::CircuitGenerator(const std::string &circuit_path, const std::vector<uint64_t>& wires_per_input_party, const std::vector<uint64_t>& wires_per_output_party) : _circuit_path(circuit_path), _wires_per_input_party(wires_per_input_party), _wires_per_output_party(wires_per_output_party) {
    _open_files();
}

gabe::circuits::generator::CircuitGenerator::~CircuitGenerator() {
    _close_files();
}

void gabe::circuits::generator::CircuitGenerator::_open_files() {
    std::string name = _circuit_path, extension = "";

    std::size_t pos = _circuit_path.find('.');
    if (pos != std::string::npos) {
        name = std::string( _circuit_path.begin(), _circuit_path.begin() + pos );
        extension = std::string( _circuit_path.begin() + pos, _circuit_path.end() );
    }

    _temp_circuit_path = name + "_temp" + extension;

    _circuit = std::ofstream(
        _circuit_path,
        std::ios::out | std::ios::trunc
    );
    _temp_circuit = std::fstream(
        _temp_circuit_path,
        std::ios::in | std::ios::out | std::ios::trunc
    );
}

void gabe::circuits::generator::CircuitGenerator::_close_files() {
    // Closes the files
    if (_circuit.is_open()) { _circuit.close(); }
    if (_temp_circuit.is_open()) { _temp_circuit.close(); }

    // Removes the temporary file from the system
    remove( _temp_circuit_path.c_str() );
}
