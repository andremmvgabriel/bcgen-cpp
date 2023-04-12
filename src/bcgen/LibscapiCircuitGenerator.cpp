#include <bcgen/LibscapiCircuitGenerator.hpp>
#include <fmt/format.h>

#include <numeric>

gabe::bcgen::LibscapiCircuitGenerator::LibscapiCircuitGenerator(const std::string &circuit_name) : CircuitGenerator("libscapi_" + circuit_name) {
    _gates_map = { {"xor", "0110"}, {"inv", "10"}, {"and", "0001"}, {"or", "0111"} };
}

gabe::bcgen::LibscapiCircuitGenerator::LibscapiCircuitGenerator(const std::string &circuit_name, const std::string &circuits_directory) : CircuitGenerator("libscapi_" + circuit_name, circuits_directory) {
    _gates_map = { {"xor", "0110"}, {"inv", "10"}, {"and", "0001"}, {"or", "0111"} };
}

void gabe::bcgen::LibscapiCircuitGenerator::_write_header(std::ofstream& file) {
    // Lambda function to construct the inputs section of the header
    auto prepare_inputs = [&](){
        std::string inputs;
        uint64_t assigned_inputs = 0x00;

        for (int i = 0 ; i < _input_parties.size() ; i++) {
            std::vector<uint64_t> wires;

            for (int j = 0 ; j < _input_parties[i] ; j++) {
                wires.push_back(assigned_inputs++);
            }
            
            inputs += fmt::format("{} {}\n{}\n\n", i+1, _input_parties[i], fmt::join(wires, "\n"));
        }

        return inputs;
    };

    // Lambda function to construct the outputs section of the header
    auto prepare_outputs = [&](){
        std::string outputs;
        uint64_t outputs_to_assign = std::accumulate(_output_parties.begin(), _output_parties.end(), 0);

        for (int i = 0 ; i < _output_parties.size() ; i++) {
            std::vector<uint64_t> wires;

            for (int j = 0 ; j < _output_parties[i] ; j++) {
                wires.push_back(_counter_wires - outputs_to_assign--);
            }

            outputs += fmt::format("{} {}\n{}\n\n", i+1, _output_parties[i], fmt::join(wires, "\n"));
        }

        return outputs;
    };
    
    // Header construction
    const std::string header = fmt::format("{} {}\n\n", _counter_gates, _input_parties.size());
    const std::string inputs = fmt::format("{}", prepare_inputs());
    const std::string outputs = fmt::format("{}", prepare_outputs());

    // Header writing
    file.write(header.c_str(), header.size());
    file.write(inputs.c_str(), inputs.size());
    file.write(outputs.c_str(), outputs.size());
}
