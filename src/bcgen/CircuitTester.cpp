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
    printf("\n> Results:\n");
    printf("-> Inputs:\n");
    for (int i = 0; i < _input_parties_wires.size(); i++) {
        printf("   > %s", fmt::format("Party {}:", i).c_str());
        for (auto wire : _input_parties_wires[i]) {
            printf(" %d", _wires[wire]);
        }
        printf("\n");
    }
    printf("-> Outputs:\n");
    for (int i = 0; i < _output_parties_wires.size(); i++) {
        printf("   > %s", fmt::format("Party {}:", i).c_str());
        for (auto wire : _output_parties_wires[i]) {
            printf(" %d", _wires[wire]);
        }
        printf("\n");
    }
}

void gabe::bcgen::CircuitTester::_print_circuit_info() {
    printf("\n> Circuit Info:\n");
    printf("-> Gates: %ld\n", _counter_gates);
    printf("-> Wires: %ld\n", _counter_wires);
    printf("-> Number input parties: %lu\n", _input_parties.size());
    for (int i = 0; i < _input_parties.size(); i++) {
        printf("   > Input party %d: %lu wires\n", i, _input_parties[i]);
    }
    printf("-> Number output parties: %lu\n", _output_parties.size());
    for (int i = 0; i < _output_parties.size(); i++) {
        printf("   > Output party %d: %lu wires\n", i, _output_parties[i]);
    }
}
