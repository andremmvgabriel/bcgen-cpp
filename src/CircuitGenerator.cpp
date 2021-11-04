#include <CircuitGenerator.hpp>

gabe::circuits::generator::CircuitGenerator::CircuitGenerator() {}

gabe::circuits::generator::CircuitGenerator::CircuitGenerator(const std::string &circuit_name, const std::vector<uint64_t>& wires_per_input_party, const std::vector<uint64_t>& wires_per_output_party, const std::string &circuits_directory) : _circuit_name(circuit_name), _temp_circuit_name(_circuit_name+"_temp"), _wires_per_input_party(wires_per_input_party), _wires_per_output_party(wires_per_output_party), _circuits_directory(circuits_directory) {
    _create_save_directory();
    _open_files();
}

gabe::circuits::generator::CircuitGenerator::~CircuitGenerator() {
    _close_files();
}

void gabe::circuits::generator::CircuitGenerator::_create_save_directory() {
    // If the user gave a directory, it is assumed that it exists and does not need to be created
    if (_circuits_directory.size() != 0) return;

    // Get the current directory and add "circuits" to it
    _circuits_directory = std::filesystem::current_path().string() + "/circuits/";

    // Create circuits directory in current directory
    _mkdir(_circuits_directory.c_str());
}

void gabe::circuits::generator::CircuitGenerator::_open_files() {
    // Open circuit file
    _circuit = std::ofstream(
        _circuits_directory + _circuit_name + ".txt",
        std::ios::out | std::ios::trunc
    );

    // Open temporary circuit file (will be deleted in the end)
    _temp_circuit = std::fstream(
        _circuits_directory + _temp_circuit_name + ".txt",
        std::ios::in | std::ios::out | std::ios::trunc
    );
}

void gabe::circuits::generator::CircuitGenerator::_close_files() {
    // Closes the files
    if (_circuit.is_open()) { _circuit.close(); }
    if (_temp_circuit.is_open()) { _temp_circuit.close(); }

    // Removes the temporary file from the system
    remove((_circuits_directory + _temp_circuit_name + ".txt").c_str() );
}
