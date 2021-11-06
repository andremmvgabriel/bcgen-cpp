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
    //remove((_circuits_directory + _temp_circuit_name + ".txt").c_str() );
}

void gabe::circuits::generator::CircuitGenerator::_assert_equal_size(const SignedVar& var1, const SignedVar& var2) {
    if (var1.number_wires != var2.number_wires)
        throw std::invalid_argument("The inserted variables do not share the same size.");
}

void gabe::circuits::generator::CircuitGenerator::_assert_equal_size(const UnsignedVar& var1, const UnsignedVar& var2) {
    if (var1.number_wires != var2.number_wires)
        throw std::invalid_argument("The inserted variables do not share the same size.");
}

void gabe::circuits::generator::CircuitGenerator::_write_1_1_gate(const uint64_t input, const uint64_t output, const std::string &gate) {
    // Line construction
    std::string line = "1 1 " + std::to_string(input) + " " + std::to_string(output) + " " + gate + "\n";

    // Writting...
    _temp_circuit.write( line.c_str(), line.size() );
}

void gabe::circuits::generator::CircuitGenerator::_write_2_1_gate(const uint64_t input1, const uint64_t input2, const uint64_t output, const std::string &gate) {
    // Input 1 should contain the smallest label input wire
    std::string in1 = input1 < input2 ? std::to_string(input1) : std::to_string(input2);

    // Input 2 should contain the biggest label input wire
    std::string in2 = input1 > input2 ? std::to_string(input1) : std::to_string(input2);

    // Line construction
    std::string line = "2 1 " + in1 + " " + in2 + " " + std::to_string(output) + " " + gate + "\n";

    // Writting...
    _temp_circuit.write( line.c_str(), line.size() );
}

void gabe::circuits::generator::CircuitGenerator::xor(const Wire& input1, const Wire& input2, Wire& output) {
    _write_2_1_gate( input1.label, input2.label, _counter_wires, _gates_map["xor"] );

    output.label = _counter_wires; // This is done here to prevent the label override if input and output are the same variable

    // Counters increment
    _counter_wires++;
    _counter_xor_gates++;
}

void gabe::circuits::generator::CircuitGenerator::and(const Wire& input1, const Wire& input2, Wire& output) {
    _write_2_1_gate( input1.label, input2.label, _counter_wires, _gates_map["and"] );

    output.label = _counter_wires; // This is done here to prevent the label override if input and output are the same variable

    // Counters increment
    _counter_wires++;
    _counter_and_gates++;
}

void gabe::circuits::generator::CircuitGenerator::inv(const Wire& input, Wire& output) {
    _write_1_1_gate( input.label, _counter_wires, _gates_map["inv"] );

    output.label = _counter_wires; // This is done here to prevent the label override if input and output are the same variable

    // Counters increment
    _counter_wires++;
    _counter_inv_gates++;
}

void gabe::circuits::generator::CircuitGenerator::or(const Wire& input1, const Wire& input2, Wire& output) {
    _write_2_1_gate( input1.label, input2.label, _counter_wires, _gates_map["or"] );

    output.label = _counter_wires; // This is done here to prevent the label override if input and output are the same variable

    // Counters increment
    _counter_wires++;
    _counter_or_gates++;
}

void gabe::circuits::generator::CircuitGenerator::xor(const SignedVar& input1, const SignedVar& input2, SignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // XOR all the wires
    for (int i = 0; i < input1.number_wires; i++)
        xor(input1.wires[i], input2.wires[i], output.wires[i]);
}

void gabe::circuits::generator::CircuitGenerator::and(const SignedVar& input1, const SignedVar& input2, SignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // AND all the wires
    for (int i = 0; i < input1.number_wires; i++)
        and(input1.wires[i], input2.wires[i], output.wires[i]);
}

void gabe::circuits::generator::CircuitGenerator::inv(const SignedVar& input, SignedVar& output) {
    // Safety checks
    _assert_equal_size(input, output);

    // INV all the wires
    for (int i = 0; i < input.number_wires; i++)
        inv(input.wires[i], output.wires[i]);
}

void gabe::circuits::generator::CircuitGenerator::xor(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // XOR all the wires
    for (int i = 0; i < input1.number_wires; i++)
        xor(input1.wires[i], input2.wires[i], output.wires[i]);
}

void gabe::circuits::generator::CircuitGenerator::and(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // AND all the wires
    for (int i = 0; i < input1.number_wires; i++)
        and(input1.wires[i], input2.wires[i], output.wires[i]);
}

void gabe::circuits::generator::CircuitGenerator::inv(const UnsignedVar& input, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input, output);

    // INV all the wires
    for (int i = 0; i < input.number_wires; i++)
        inv(input.wires[i], output.wires[i]);
}

void gabe::circuits::generator::CircuitGenerator::or(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // OR all the wires
    for (int i = 0; i < input1.number_wires; i++)
        or(input1.wires[i], input2.wires[i], output.wires[i]);
}
