#include <bcgen/CircuitTester.hpp>
#include <fmt/format.h>

#include <numeric>
#include <stdexcept>
#include <filesystem>

std::vector<std::string> gabe::bcgen::CircuitTester::_split_str(std::string word, std::string delimiter) {
    // Split result
    std::vector<std::string> output;

    // Splitting...
    // > Keep splitting until no more delimiters are found
    size_t index = word.find(delimiter);
    while (index != -1) {
        // Adds the current split part to the output result
        output.push_back( word.substr(0, index) );

        // Updates the word for next search
        word = word.substr(index + 1);

        // Finds the next index with the delimiter
        index = word.find(delimiter);
    }

    // Left string is the last part of the splitting
    output.push_back(word);

    return output;
}

gabe::bcgen::CircuitTester::~CircuitTester() {
    if (_circuit.is_open()) {
        _circuit.close();
    }
}

void gabe::bcgen::CircuitTester::open(const std::string &circuit_name, const std::string& circuit_directory) {
    // Safety - Closes a previously opened circuit file, in case there is one opened 
    if (_circuit.is_open()) {
        _circuit.close();
    }

    // Caches the name and directory of the circuit
    _circuit_name = circuit_name;
    _circuits_directory = std::filesystem::absolute(circuit_directory);

    // Restarts the executions counter
    _counter_executions = 0;

    // Open the circuit file
    std::string circuit_path = std::filesystem::absolute(circuit_directory) / (circuit_name + ".txt");
    _circuit = std::ifstream( circuit_path, std::ios::in );

    // Safety check
    if (_circuit.fail() || !_circuit.is_open()) {
        // Error message
        const std::string error_msg = fmt::format("Failed to open circuit file. Path to circuit: {}", circuit_path);

        // Raises the error
        throw std::runtime_error(error_msg);
    }

    _read_header();
}

void gabe::bcgen::CircuitTester::run(const std::vector<uint8_t> &inputs) {
    // Safety check
    // > The given inputs should have the total size of all the input parties of the circuit
    if (inputs.size() != std::accumulate(_input_parties.begin(), _input_parties.end(), 0)) {
        // Error message
        const std::string error_msg = fmt::format(
            "Wrong amount of inputs given to the circuit (given/expected: {}/{}).",
            inputs.size(), _wires.size()
        );

        // Raises the error
        throw std::runtime_error(error_msg);
    }

    // Assigns the input values to their respective wires
    for (int i = 0 ; i < inputs.size() ; i++) {
        _wires[i] = inputs[i];
    }

    // 
    std::string line;
    for (int i = 0; i < _counter_gates; i++) {
        std::getline(_circuit, line);

        std::vector<std::string> parts = _split_str(line, " ");

        _wires[i + inputs.size()] = _gates_operations[parts[parts.size()-1]](parts);
    }

    // Return the reading pointer back to the beginning of the circuit
    _circuit.seekg(_circuit_start_pos);

    // Show results to the user
    _print_results();
}

void gabe::bcgen::CircuitTester::_print_results() {
    printf("%s\n", fmt::format("+ {:^78} +", fmt::format(">>> Execution {} <<<", ++_counter_executions)).c_str());
    printf("%s\n", fmt::format("+ {:<78} +", "Inputs:").c_str());
    for (int i = 0; i < _input_parties.size(); i++) {
        std::string word;
        for (auto wire : _input_parties_wires[i]) {
            word += std::to_string(_wires[wire]);
        }
        printf("%s\n", fmt::format("+--> P{:<5}: {:<67} +", i, word).c_str());
    }
    printf("%s\n", fmt::format("+ {:<78} +", "Outputs:").c_str());
    for (int i = 0; i < _output_parties.size(); i++) {
        std::string word;
        for (auto wire : _output_parties_wires[i]) {
            word += std::to_string(_wires[wire]);
        }
        printf("%s\n", fmt::format("+--> P{:<5}: {:<67} +", i, word).c_str());
    }
    printf("%s\n", fmt::format("+{}+", std::string(80, '-')).c_str());
}

void gabe::bcgen::CircuitTester::_print_circuit_info() {
    printf("\n");
    printf("%s\n", fmt::format("+{}+", std::string(80, '-')).c_str());
    printf("%s\n", fmt::format("+ Name: {:<72} +", _circuit_name).c_str());
    printf("%s\n", fmt::format("+ Path: {:<72} +", _circuits_directory).c_str());
    printf("%s\n", fmt::format("+{}+", std::string(80, ' ')).c_str());
    printf("%s\n", fmt::format("+ Total gates: {:<65} +", _counter_gates).c_str());
    printf("%s\n", fmt::format("+ Total wires: {:<65} +", _counter_wires).c_str());
    printf("%s\n", fmt::format("+{}+", std::string(80, ' ')).c_str());
    printf("%s\n", fmt::format("+ {:^8} + {:^32} + {:^32} +", "Party", "Inputs sizes", "Output sizes").c_str());
    int biggest = _input_parties.size() > _output_parties.size() ? _input_parties.size() : _output_parties.size();
    for (int i = 0; i < biggest; i++) {
        printf("%s\n", fmt::format("+ {:^8} | {:^32} | {:^32} +",
            i,
            i >= _input_parties.size() ? "-" : std::to_string(_input_parties[i]),
            i >= _output_parties.size() ? "-" : std::to_string(_output_parties[i])
        ).c_str());
    }
    printf("%s\n", fmt::format("+{}+", std::string(80, '-')).c_str());
}
