#include <bcgen/version.hpp>
#include <bcgen/CircuitGenerator.hpp>

gabe::bcgen::CircuitGenerator::CircuitGenerator(const std::string &circuit_name) : _circuit_name(circuit_name), _circuits_directory("circuits") {
    _setup_logger();
    _create_save_directory();
}

gabe::bcgen::CircuitGenerator::CircuitGenerator(const std::string &circuit_name, const std::string &circuits_directory) : _circuit_name(circuit_name), _circuits_directory(circuits_directory) {
    _setup_logger();
    _create_save_directory();
}

gabe::bcgen::CircuitGenerator::~CircuitGenerator() {
    std::remove((_circuits_directory / (_circuit_name + "_temp.txt")).c_str());
}

void gabe::bcgen::CircuitGenerator::_setup_logger() {
    _logger = spdlog::basic_logger_mt("bcgen_" + _circuit_name, "logs/" + _circuit_name + ".txt", true);
    _logger->set_level(spdlog::level::trace);
    _logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%=7l] %v");

    // Logging
    _logger->info(std::string(48, '*'));
    _logger->info("Boolean Circuit Generator V{}", gabe::bcgen::get_version());
    _logger->info(std::string(48, '*'));
}

void gabe::bcgen::CircuitGenerator::_create_save_directory() {
    // There is nothing to be created if the specified directory is empty
    if (_circuits_directory.empty()) return;

    // Create circuits directory
    if (std::filesystem::create_directory(_circuits_directory)) {
        // Logging
        _logger->info("Circuits directory created.");
    }

    // Logging
    _logger->debug(
        fmt::format("> Circuits will be generated at: {}", std::filesystem::absolute(_circuits_directory).c_str())
    );
}

void gabe::bcgen::CircuitGenerator::_write_header(std::ofstream& file) {}

void gabe::bcgen::CircuitGenerator::_write_circuit(std::ofstream& file) {
    // Open the temporary circuit file
    std::ifstream temp_circuit(
        _circuits_directory / (_circuit_name + "_temp.txt"),
        std::ios::in
    );

    // Can only move temp contents if file was created
    if (!temp_circuit.fail()) {
        // Contents transfer
        std::string line;
        while (std::getline(temp_circuit, line)) {
            line += "\n";
            file.write(line.c_str(), line.size());
        }

        // Logging
        _logger->info("Successfully transfered contents from temporary circuit file.");
    } else {
        // Logging
        _logger->info("Cannot transfer contents from temporary circuit file. It was not created.");
    }

    // Writes unflushed data inside the buffer 
    _flush_buffer(file);

    // Logging
    _logger->info("Circuit writing phase completed.");
}

void gabe::bcgen::CircuitGenerator::_assert_add_input(uint64_t size) {
    if (_counter_wires + size > _expected_input_wires) {
        const std::string error_msg = fmt::format("There aren't enough input wires available to add an input of size {}.", size);
        _logger->error(error_msg);
        throw std::runtime_error(error_msg);
    }
}

void gabe::bcgen::CircuitGenerator::_assert_equal_size(const SignedVar& var1, const SignedVar& var2) {
    if (var1.size() != var2.size()) {
        const std::string error_msg = "Variables should have the same size to perform operation.";
        _logger->error(error_msg);
        throw std::runtime_error(error_msg);
    }
}

void gabe::bcgen::CircuitGenerator::_assert_equal_size(const UnsignedVar& var1, const UnsignedVar& var2) {
    if (var1.size() != var2.size()) {
        const std::string error_msg = "Variables should have the same size to perform operation.";
        _logger->error(error_msg);
        throw std::runtime_error(error_msg);
    }
}

void gabe::bcgen::CircuitGenerator::_assert_equal_size(const SignedVar& var, const uint64_t size) {
    if (var.size() != size) {
        const std::string error_msg = fmt::format("Variable should have a size of {}.", size);
        _logger->error(error_msg);
        throw std::runtime_error(error_msg);
    }
}

void gabe::bcgen::CircuitGenerator::_assert_equal_size(const UnsignedVar& var, const uint64_t size) {
    if (var.size() != size){
        const std::string error_msg = fmt::format("Variable should have a size of {}.", size);
        _logger->error(error_msg);
        throw std::runtime_error(error_msg);
    }
}

void gabe::bcgen::CircuitGenerator::_flush_buffer(std::ofstream& file) {
    // Writes all the buffer lines into the file
    for (auto & line : _buffer) {
        file.write(line.c_str(), line.size());
    }

    // Clears the buffer
    _buffer.clear();
    _buffer_size = 0;
}

void gabe::bcgen::CircuitGenerator::_write_gate(const std::string& line, const std::string& gate) {
    // Adds the gate line into the buffer
    _buffer.push_back(line);
    _buffer_size += line.size();

    // Buffer flush check
    if (_buffer_size >= _buffer_max_size) {
        // Open the temporary circuit file
        std::ofstream temp_circuit(
            _circuits_directory / (_circuit_name + "_temp.txt"),
            std::ios::out | std::ios::app
        );

        // Flushes the buffer into the temporary file
        _flush_buffer(temp_circuit);
    }

    // Increments the counters
    _counter_gates++;
    _gates_counters[gate]++;
}

void gabe::bcgen::CircuitGenerator::_write_1_1_gate(const uint64_t input, const uint64_t output, const std::string &gate) {
    // Line construction
    const std::string line = fmt::format("1 1 {} {} {}\n", input, output, gate);
    _write_gate(line, gate);
}

void gabe::bcgen::CircuitGenerator::_write_2_1_gate(const uint64_t input1, const uint64_t input2, const uint64_t output, const std::string &gate) {
    // Line construction
    const std::string line = fmt::format("2 1 {} {} {} {}\n", input1 < input2 ? input1 : input2, input1 < input2 ? input2 : input1, output, gate);
    _write_gate(line, gate);
}

void gabe::bcgen::CircuitGenerator::limit_buffer(uint64_t size) {
    _buffer_max_size = size;
    _logger->info(fmt::format("Circuit generator buffer is now limited to {} bytes.", size));
}

void gabe::bcgen::CircuitGenerator::add_input_party(uint64_t size) {
    _input_parties.push_back(size);
    _expected_input_wires += size;

    // Logging
    _logger->info("Added input party {} with {} wires.", _input_parties.size(), size);
    _logger->debug("> Party {} assigned wires: [{}:{}]", _input_parties.size(), _expected_input_wires - size, _expected_input_wires - 1);
}

void gabe::bcgen::CircuitGenerator::add_output_party(uint64_t size) {
    _output_parties.push_back(size);
    _expected_output_wires += size;

    // Logging
    _logger->info("Added output party {} with {} wires.", _output_parties.size(), size);
}

void gabe::bcgen::CircuitGenerator::add_input(Wire& wire) {
    // Safety check
    _assert_add_input(1);
    
    // Assigns a label to the wire
    wire.label = _counter_wires++;
}

void gabe::bcgen::CircuitGenerator::add_input(SignedVar& variable) {
    // Safety check
    _assert_add_input(variable.size());

    for (int i = 0; i < variable.size(); i++)
        add_input( variable[i] );
}

void gabe::bcgen::CircuitGenerator::add_input(UnsignedVar& variable) {
    // Safety check
    _assert_add_input(variable.size());
    
    for (int i = 0; i < variable.size(); i++)
        add_input( variable[i] );
}

void gabe::bcgen::CircuitGenerator::add_output(Wire& wire) {}

void gabe::bcgen::CircuitGenerator::add_output(SignedVar& variable) {
    for (int i = 0; i < variable.size(); i++)
        add_output( variable[i] );
}

void gabe::bcgen::CircuitGenerator::add_output(UnsignedVar& variable) {
    for (int i = 0; i < variable.size(); i++)
        add_output( variable[i] );
}

void gabe::bcgen::CircuitGenerator::start() {
    // Safety check - No input wires
    if (!_expected_input_wires) {
        const std::string error_msg = "There are no input wires defined.";
        _logger->error(error_msg);
        throw std::runtime_error(error_msg);
    }

    // Safety check - Dead wires
    if (_counter_wires < _expected_input_wires) {
        const std::string error_msg = "Dead wires in the circuit. There are input wires that are not assigned to a variable.";
        _logger->error(error_msg);
        throw std::runtime_error(error_msg);
    }

    // Creates the zero and one wires
    XOR( Wire(), Wire(), _zero_wire );
    INV( _zero_wire, _one_wire ); 

    // Logging
    _logger->info("Starting the circuit file creation...");
}

void gabe::bcgen::CircuitGenerator::stop() {
    // Open circuit file
    std::ofstream circuit(
        _circuits_directory / (_circuit_name + ".txt"),
        std::ios::out | std::ios::trunc
    );

    // Safety check
    if (circuit.fail()) {
        const std::string error_msg = "Failed to write circuit. Cannot open file.";
        _logger->error(error_msg);
        throw std::runtime_error(error_msg);
    }

    // Writing phase
    _write_header(circuit);
    _write_circuit(circuit);

    // Closes the circuit file
    circuit.close();

    // Logging
    _logger->info("Successfully created circuit file.");
    _logger->debug("> Total gates: {}", _counter_gates);
    _logger->trace("-> OR: {}", _gates_counters[_gates_map["or"]]);
    _logger->trace("-> XOR: {}", _gates_counters[_gates_map["xor"]]);
    _logger->trace("-> AND: {}", _gates_counters[_gates_map["and"]]);
    _logger->trace("-> INV: {}", _gates_counters[_gates_map["inv"]]);
    _logger->debug("> Total wires: {}", _counter_wires);
}

void gabe::bcgen::CircuitGenerator::assign_value(SignedVar& variable, int64_t value) {
    for (uint8_t i = 0; i < variable.size(); i++) {
        // Gets the current bit value
        uint8_t bit_value = (value >> i) & 0x01;

        // Assigns the zero or one wire label to the current wire
        variable[i].label = bit_value == 0 ? _zero_wire.label : _one_wire.label;
    }
}

void gabe::bcgen::CircuitGenerator::assign_value(UnsignedVar& variable, uint64_t value) {
    for (uint8_t i = 0; i < variable.size(); i++) {
        // Gets the current bit value
        uint8_t bit_value = (value >> i) & 0x01;

        // Assigns the zero or one wire label to the current wire
        variable[i].label = bit_value == 0 ? _zero_wire.label : _one_wire.label;
    }
}

void gabe::bcgen::CircuitGenerator::shift_left(SignedVar &variable, uint64_t amount) {
    // Shifting
    for (uint64_t i = variable.size() - 1; i >= amount; i--)
        variable[i] = variable[i - amount];
    
    // Assign 0 to the new wires
    for (uint64_t i = 0; i < amount && i < variable.size(); i++)
        variable[i] = _zero_wire;
}

void gabe::bcgen::CircuitGenerator::shift_left(UnsignedVar &variable, uint64_t amount) {
    // Shifting
    for (uint64_t i = variable.size() - 1; i >= amount; i--)
        variable[i] = variable[i - amount];
    
    // Assign 0 to the new wires
    for (uint64_t i = 0; i < amount && i < variable.size(); i++)
        variable[i] = _zero_wire;
}

void gabe::bcgen::CircuitGenerator::shift_right(SignedVar &variable, uint64_t amount) {
    // Shifting
    for (int64_t i = 0; i < (int64_t)variable.size() - (int64_t)amount; i++)
        variable[i] = variable[i + amount];
    
    // Assign 0 to the new wires
    for (int64_t i = variable.size() - 1; i > (int64_t)variable.size() - 1 - (int64_t)amount && i >= 0; i--)
        variable[i] = _zero_wire;
}

void gabe::bcgen::CircuitGenerator::shift_right(UnsignedVar &variable, uint64_t amount) {
    // Shifting
    for (int64_t i = 0; i < (int64_t)variable.size() - (int64_t)amount; i++)
        variable[i] = variable[i + amount];
    
    // Assign 0 to the new wires
    for (int64_t i = variable.size() - 1; i > (int64_t)variable.size() - 1 - (int64_t)amount && i >= 0; i--)
        variable[i] = _zero_wire;
}

void gabe::bcgen::CircuitGenerator::shift_left(const SignedVar& variable, uint64_t amount, SignedVar& output) {
    output = variable;
    shift_left(output, amount);
}

void gabe::bcgen::CircuitGenerator::shift_left(const UnsignedVar& variable, uint64_t amount, UnsignedVar& output) {
    output = variable;
    shift_left(output, amount);
}

void gabe::bcgen::CircuitGenerator::shift_right(const SignedVar& variable, uint64_t amount, SignedVar& output) {
    output = variable;
    shift_right(output, amount);
}

void gabe::bcgen::CircuitGenerator::shift_right(const UnsignedVar& variable, uint64_t amount, UnsignedVar& output) {
    output = variable;
    shift_right(output, amount);
}

void gabe::bcgen::CircuitGenerator::rotate_left(SignedVar &variable, uint64_t amount) {
    SignedVar temp = variable;

    for (uint64_t i = 0; i < variable.size(); i++) {
        uint64_t j = (i + amount) % variable.size();
        variable[i] = temp[j];
    }
}

void gabe::bcgen::CircuitGenerator::rotate_left(UnsignedVar &variable, uint64_t amount) {
    UnsignedVar temp = variable;

    for (uint64_t i = 0; i < variable.size(); i++) {
        uint64_t j = (i + amount) % variable.size();
        variable[i] = temp[j];
    }
}

void gabe::bcgen::CircuitGenerator::rotate_right(SignedVar &variable, uint64_t amount) {
    SignedVar temp = variable;

    for (uint64_t i = 0; i < variable.size(); i++) {
        uint64_t j = (i + amount) % variable.size();
        variable[j] = temp[i];
    }
}

void gabe::bcgen::CircuitGenerator::rotate_right(UnsignedVar &variable, uint64_t amount) {
    UnsignedVar temp = variable;

    for (uint64_t i = 0; i < variable.size(); i++) {
        uint64_t j = (i + amount) % variable.size();
        variable[j] = temp[i];
    }
}

void gabe::bcgen::CircuitGenerator::rotate_left(const SignedVar& variable, uint64_t amount, SignedVar& output) {
    output = variable;
    rotate_left(output, amount);
}

void gabe::bcgen::CircuitGenerator::rotate_left(const UnsignedVar& variable, uint64_t amount, UnsignedVar& output) {
    output = variable;
    rotate_left(output, amount);
}

void gabe::bcgen::CircuitGenerator::rotate_right(const SignedVar& variable, uint64_t amount, SignedVar& output) {
    output = variable;
    rotate_right(output, amount);
}

void gabe::bcgen::CircuitGenerator::rotate_right(const UnsignedVar& variable, uint64_t amount, UnsignedVar& output) {
    output = variable;
    rotate_right(output, amount);
}

void gabe::bcgen::CircuitGenerator::flip(SignedVar& variable) {
    SignedVar temp = variable;

    for (uint64_t i = 0; i < variable.size(); i++)
        variable[i] = temp[variable.size() - 1 - i];
}

void gabe::bcgen::CircuitGenerator::flip(UnsignedVar& variable) {
    UnsignedVar temp = variable;

    for (uint64_t i = 0; i < variable.size(); i++)
        variable[i] = temp[variable.size() - 1 - i];
}

void gabe::bcgen::CircuitGenerator::flip(const SignedVar& variable, SignedVar& output) {
    output = variable;
    flip(output);
}

void gabe::bcgen::CircuitGenerator::flip(const UnsignedVar& variable, UnsignedVar& output) {
    output = variable;
    flip(output);
}

void gabe::bcgen::CircuitGenerator::twos_complement(SignedVar& variable) {
    // TODO - Complete this function...
}

void gabe::bcgen::CircuitGenerator::twos_complement(UnsignedVar& variable) {
    // Variable creations
    UnsignedVar one(variable.size());

    // Variable value initializations
    assign_value(one, 1);

    // Algorithm
    INV(variable, variable);
    addition(one, variable, variable);
}

void gabe::bcgen::CircuitGenerator::twos_complement(const SignedVar& variable, SignedVar& output) {
    output = variable;
    twos_complement(output);
}

void gabe::bcgen::CircuitGenerator::twos_complement(const UnsignedVar& variable, UnsignedVar& output) {
    output = variable;
    twos_complement(output);
}

void gabe::bcgen::CircuitGenerator::XOR(const Wire input1, const Wire input2, Wire& output) {
    _write_2_1_gate( input1.label, input2.label, output.label = _counter_wires++, _gates_map["xor"] );
}

void gabe::bcgen::CircuitGenerator::AND(const Wire input1, const Wire input2, Wire& output) {
    _write_2_1_gate( input1.label, input2.label, output.label = _counter_wires++, _gates_map["and"] );
}

void gabe::bcgen::CircuitGenerator::INV(const Wire input, Wire& output) {
    _write_1_1_gate( input.label, output.label = _counter_wires++, _gates_map["inv"] );
}

void gabe::bcgen::CircuitGenerator::OR(const Wire input1, const Wire input2, Wire& output) {
    _write_2_1_gate( input1.label, input2.label, output.label = _counter_wires++, _gates_map["or"] );
}

void gabe::bcgen::CircuitGenerator::XOR(const SignedVar& input1, const SignedVar& input2, SignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // XOR all the wires
    for (int i = 0; i < input1.size(); i++)
        XOR(input1[i], input2[i], output[i]);
}

void gabe::bcgen::CircuitGenerator::AND(const SignedVar& input1, const SignedVar& input2, SignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // AND all the wires
    for (int i = 0; i < input1.size(); i++)
        AND(input1[i], input2[i], output[i]);
}

void gabe::bcgen::CircuitGenerator::INV(const SignedVar& input, SignedVar& output) {
    // Safety checks
    _assert_equal_size(input, output);

    // INV all the wires
    for (int i = 0; i < input.size(); i++)
        INV(input[i], output[i]);
}

void gabe::bcgen::CircuitGenerator::OR(const SignedVar& input1, const SignedVar& input2, SignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // OR all the wires
    for (int i = 0; i < input1.size(); i++)
        OR(input1[i], input2[i], output[i]);
}

void gabe::bcgen::CircuitGenerator::XOR(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // XOR all the wires
    for (int i = 0; i < input1.size(); i++)
        XOR(input1[i], input2[i], output[i]);
}

void gabe::bcgen::CircuitGenerator::AND(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // AND all the wires
    for (int i = 0; i < input1.size(); i++)
        AND(input1[i], input2[i], output[i]);
}

void gabe::bcgen::CircuitGenerator::INV(const UnsignedVar& input, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input, output);

    // INV all the wires
    for (int i = 0; i < input.size(); i++)
        INV(input[i], output[i]);
}

void gabe::bcgen::CircuitGenerator::OR(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // OR all the wires
    for (int i = 0; i < input1.size(); i++)
        OR(input1[i], input2[i], output[i]);
}

//void gabe::bcgen::CircuitGenerator::multiplication(const SignedVar& input1, const SignedVar& input2, SignedVar& output) {}

void gabe::bcgen::CircuitGenerator::addition(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // Variable creations
    UnsignedVar carry(output.size());
    UnsignedVar xor_a_b(output.size()); // Consider as: d
    UnsignedVar and_d_c(output.size());
    UnsignedVar and_a_b(output.size());

    // Variable value initializations
    assign_value(carry, 0);

    // Middle operations
    for (int i = 0; i < output.size(); i++) {
        XOR(input1[i], input2[i], xor_a_b[i]);

        // Do not perform these operations if it is the last cycle
        if (i != output.size() - 1) {
            AND(input1[i], input2[i], and_a_b[i]);
            AND(xor_a_b[i], carry[i], and_d_c[i]);
            OR(and_a_b[i], and_d_c[i], carry[i + 1]);
        }
    }

    // Final operations (only done like this to put the output wires last in the writting phase)
    // TODO - Think of a solution to make this possible without having this separated for cycle
    for (int i = 0; i < output.size(); i++)
        XOR(xor_a_b[i], carry[i], output[i]);
}

void gabe::bcgen::CircuitGenerator::subtraction(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // Variable creations
    UnsignedVar carry(output.size());
    UnsignedVar xor_a_b(output.size()); // Consider as: d
    UnsignedVar inv_a(output.size()); // Consider as: e
    UnsignedVar inv_d(output.size());
    UnsignedVar and_d_c(output.size());
    UnsignedVar and_a_c(output.size());

    // Variable value initializations
    assign_value(carry, 0);

    // Middle operations
    for (int i = 0; i < output.size(); i++) {
        XOR(input1[i], input2[i], xor_a_b[i]);

        // Do not perform these operations if it is the last cycle
        if (i != output.size() - 1) {
            INV(xor_a_b[i], inv_d[i]);
            INV(input1[i], inv_a[i]);
            AND(inv_d[i], carry[i], and_d_c[i]);
            AND(inv_a[i], input2[i], and_a_c[i]);
            OR(and_d_c[i], and_a_c[i], carry[i]);
        }
    }

    // Final operations (only done like this to put the output wires last in the writting phase)
    // TODO - Think of a solution to make this possible without having this separated for cycle
    for (int i = 0; i < output.size(); i++)
        XOR(xor_a_b[i], carry[i], output[i]);
}

void gabe::bcgen::CircuitGenerator::multiplication(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    // TODO - Think of any safety checks

    // Variables for multiplication calculation
    std::vector<UnsignedVar> variables(input2.size(), UnsignedVar(output.size())); // As many as the size of the input2 variable

    // Variables creation
    for (int i = 0; i < input2.size(); i++) {
        assign_value(variables.at(i), 0);

        // Variable is 0 or equal to input2 (shifted j, where j is the iteration step)
        for (int j = 0; j < input1.size(); j++)
            AND(input1[j], input2[i], variables.at(i)[i+j]);
    }

    // Final operations
    for (int i = 1; i < variables.size(); i++) {
        if (i == 1)
            addition(variables.at(i - 1), variables.at(i), output);
        else
            addition(output, variables.at(i), output);
    }
}

void gabe::bcgen::CircuitGenerator::division(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output_quotient, UnsignedVar& output_remainder) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output_quotient);
    _assert_equal_size(input1, output_remainder);

    // TODO - Temporary like this...
    assign_value(output_remainder, 0);

    // Variable creations
    UnsignedVar zero(input1.size());
    UnsignedVar substractor(input1.size());

    // Variable value initializations
    assign_value(zero, 0);

    // Control wires
    Wire control;

    for (int i = 0; i < input1.size(); i++) {
        shift_left(output_remainder, 1);
        output_remainder[0] = input1[input1.size() - 1 - i];

        greater_or_equal(output_remainder, input2, control);
        multiplexer(control, zero, input2, substractor);
        subtraction(output_remainder, substractor, output_remainder); 

        output_quotient[input1.size() - 1 - i] = control;
    }
}

void gabe::bcgen::CircuitGenerator::division_quotient(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output_quotient) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output_quotient);

    // Variable creations
    UnsignedVar remainder(output_quotient.size());
    UnsignedVar zero(input1.size());
    UnsignedVar substractor(input1.size());

    // Variable value initializations
    assign_value(remainder, 0);
    assign_value(zero, 0);

    // Control wires
    Wire control;

    for (int i = 0; i < input1.size(); i++) {
        shift_left(remainder, 1);
        remainder[0] = input1[input1.size() - 1 - i];

        greater_or_equal(remainder, input2, control);

        // This is done like this because the remainder isn't of importance in the very last iteration (Otherwise we would be writting unnecessary operations in the circuit file)
        if (i != input1.size() - 1) {
            multiplexer(control, zero, input2, substractor);
            subtraction(remainder, substractor, remainder);
        }

        output_quotient[input1.size() - 1 - i] = control;
    }
}

void gabe::bcgen::CircuitGenerator::division_remainder(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output_remainder) {
    UnsignedVar quotient(output_remainder.size());

    division(input1, input2, quotient, output_remainder);
}

void gabe::bcgen::CircuitGenerator::multiplexer(const Wire& control, const SignedVar& input1, const SignedVar& input2, SignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    Wire not_control;
    INV(control, not_control);

    SignedVar and_in1(input1.size());
    SignedVar and_in2(input2.size());

    for (int i = 0; i < input1.size(); i++) {
        AND(not_control, input1[i], and_in1[i]);
        AND(control, input2[i], and_in2[i]);
    }

    // Final operation (only done like this to put the output wires last in the writting phase)
    // TODO - Think of a solution to make this possible without having this separated for cycle
    for (int i = 0; i < input1.size(); i++)
        OR(and_in1[i], and_in2[i], output[i]);
}

void gabe::bcgen::CircuitGenerator::multiplexer(const SignedVar& control, const SignedVar& input1, const SignedVar& input2, SignedVar& output) {
    // Safety checks
    _assert_equal_size(control, 1);

    multiplexer(control[0], input1, input2, output);
}

void gabe::bcgen::CircuitGenerator::equal(const SignedVar& input1, const SignedVar& input2, Wire& output) {
    // Safety checks
    _assert_equal_size(input1, input2);

    SignedVar inputs_xor(input1.size());
    XOR(input1, input2, inputs_xor);

    // Already adds the first wire from the xor to the output
    output = inputs_xor[0];

    // ORs every single wire (if more than 1)
    for (int i = 1; i < inputs_xor.size(); i++)
        OR(inputs_xor[i], output, output);

    // Inverts the output result (Until here the output is 1 every time wires i-th from the inputs differ)
    INV(output, output);
}

void gabe::bcgen::CircuitGenerator::equal(const SignedVar& input1, const SignedVar& input2, SignedVar& output) {
    // Safety checks
    _assert_equal_size(output, 1);

    equal(input1, input1, output[0]);
}

void gabe::bcgen::CircuitGenerator::multiplexer(const Wire& control, const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    Wire not_control;
    INV(control, not_control);

    UnsignedVar and_in1(input1.size());
    UnsignedVar and_in2(input2.size());

    for (int i = 0; i < input1.size(); i++) {
        AND(not_control, input1[i], and_in1[i]);
        AND(control, input2[i], and_in2[i]);
    }

    // Final operation (only done like this to put the output wires last in the writting phase)
    // TODO - Think of a solution to make this possible without having this separated for cycle
    for (int i = 0; i < input1.size(); i++)
        OR(and_in1[i], and_in2[i], output[i]);
}

void gabe::bcgen::CircuitGenerator::multiplexer(const UnsignedVar& control, const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(control, 1);

    multiplexer(control[0], input1, input2, output);
}

void gabe::bcgen::CircuitGenerator::equal(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output) {
    // Safety checks
    _assert_equal_size(input1, input2);

    UnsignedVar inputs_xor(input1.size());
    XOR(input1, input2, inputs_xor);

    // Already adds the first wire from the xor to the output
    output = inputs_xor[0];

    // ORs every single wire (if more than 1)
    for (int i = 1; i < inputs_xor.size(); i++)
        OR(inputs_xor[i], output, output);

    // Inverts the output result (Until here the output is 1 every time wires i-th from the inputs differ)
    INV(output, output);
}

void gabe::bcgen::CircuitGenerator::equal(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(output, 1);

    equal(input1, input1, output[0]);
}

void gabe::bcgen::CircuitGenerator::greater(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output) {
    // TODO - Make a schematic in docs for this function, so people understand the following lines

    // Safety checks
    _assert_equal_size(input1, input2);

    UnsignedVar not_input2(input2.size());
    INV(input2, not_input2);

    UnsignedVar inputs_xnor(input1.size());
    // This is done like this instead of a simple xor and inv to the whole variables because the least significant wire from the inputs is not relevant for the xnor
    for (int i = 1; i < input1.size(); i++) {
        XOR(input1[i], input2[i], inputs_xnor[i]);
        INV(inputs_xnor[i], inputs_xnor[i]);
    }

    // Performs the operations between the ORs (see schematic)
    UnsignedVar middle_operations(input1.size());
    for (int i = 0; i < middle_operations.size(); i++) {
        for (int j = i; j < middle_operations.size(); j++) {
            if (i == j)
                AND(input1[j], not_input2[i], middle_operations[i]);
            else
                AND(middle_operations[i], inputs_xnor[j], middle_operations[i]);
        }
    }

    // Already adds the first wire from the middle operations to the output
    output = middle_operations[0];

    // Perfoms the final operations - ORs every single wire (if more than 1)
    for (int i = 1; i < middle_operations.size(); i++)
        OR(middle_operations[i], output, output);
}

void gabe::bcgen::CircuitGenerator::greater(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(output, 1);

    greater(input1, input2, output[0]);
}

void gabe::bcgen::CircuitGenerator::smaller(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output) {
    // TODO - Make a schematic in docs for this function, so people understand the following lines

    // Safety checks
    _assert_equal_size(input1, input2);

    UnsignedVar not_input1(input1.size());
    INV(input1, not_input1);

    UnsignedVar inputs_xnor(input1.size());
    // This is done like this instead of a simple xor and inv to the whole variables because the least significant wire from the inputs is not relevant for the xnor
    for (int i = 1; i < input1.size(); i++) {
        XOR(input1[i], input2[i], inputs_xnor[i]);
        INV(inputs_xnor[i], inputs_xnor[i]);
    }

    // Performs the operations between the ORs (see schematic)
    UnsignedVar middle_operations(input1.size());
    for (int i = 0; i < middle_operations.size(); i++) {
        for (int j = i; j < middle_operations.size(); j++) {
            if (i == j)
                AND(not_input1[j], input2[i], middle_operations[i]);
            else
                AND(middle_operations[i], inputs_xnor[j], middle_operations[i]);
        }
    }

    // Already adds the first wire from the middle operations to the output
    output = middle_operations[0];

    // Perfoms the final operations - ORs every single wire (if more than 1)
    for (int i = 1; i < middle_operations.size(); i++)
        OR(middle_operations[i], output, output);
}

void gabe::bcgen::CircuitGenerator::smaller(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(output, 1);

    smaller(input1, input2, output[0]);
}

void gabe::bcgen::CircuitGenerator::greater_or_equal(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output) {
    // Safety checks
    _assert_equal_size(input1, input2);

    smaller(input1, input2, output);
    INV(output, output);
}

void gabe::bcgen::CircuitGenerator::greater_or_equal(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(output, 1);

    greater_or_equal(input1, input2, output[0]);
}

void gabe::bcgen::CircuitGenerator::smaller_or_equal(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output) {
    // Safety checks
    _assert_equal_size(input1, input2);

    greater(input1, input2, output);
    INV(output, output);
}

void gabe::bcgen::CircuitGenerator::smaller_or_equal(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(output, 1);

    smaller_or_equal(input1, input2, output[0]);
}

void gabe::bcgen::CircuitGenerator::comparator(const UnsignedVar& input1, const UnsignedVar& input2, Wire& out_equal, Wire& out_greater, Wire &out_smaller) {
    // Safety checks
    _assert_equal_size(input1, input2);

    // Performs the initial operations
    UnsignedVar not_input1(input1.size());
    UnsignedVar not_input2(input2.size());
    INV(input1, not_input1);
    INV(input2, not_input2);

    UnsignedVar inputs_xnor(input1.size());
    // This is done like this instead of a simple xor and inv to the whole variables because the least significant wire from the inputs is not relevant for the xnor
    for (int i = 1; i < input1.size(); i++) {
        XOR(input1[i], input2[i], inputs_xnor[i]);
        INV(inputs_xnor[i], inputs_xnor[i]);
    }

    // Performs the operations between the ORs for both greater and smaller cases (see schematics)
    UnsignedVar middle_operations_greater(input1.size());
    UnsignedVar middle_operations_smaller(input1.size());
    for (int i = 0; i < middle_operations_greater.size(); i++) {
        for (int j = i; j < middle_operations_greater.size(); j++) {
            if (i == j) {
                AND(input1[j], not_input2[i], middle_operations_greater[i]);
                AND(not_input1[j], input2[i], middle_operations_smaller[i]);
            }
            else {
                AND(middle_operations_greater[i], inputs_xnor[j], middle_operations_greater[i]);
                AND(middle_operations_smaller[i], inputs_xnor[j], middle_operations_smaller[i]);
            }
        }
    }

    // Already adds the first wire from the middle operations to each output
    out_greater = middle_operations_greater[0];
    out_smaller = middle_operations_smaller[0];

    // Perfoms the final operations - ORs every single wire (if more than 1)
    for (int i = 1; i < middle_operations_greater.size(); i++) {
        OR(middle_operations_greater[i], out_greater, out_greater);
        OR(middle_operations_smaller[i], out_smaller, out_smaller);
    }

    OR(out_greater, out_smaller, out_equal);
    INV(out_equal, out_equal);

    // TODO - I can't make this function have the very last wires as out_greater, out_smaller, and out_equal, as it requires the OR operation of out_greater and out_smaller to get the out_equal.
}

void gabe::bcgen::CircuitGenerator::comparator(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& out_equal, UnsignedVar& out_greater, UnsignedVar &out_smaller) {
    // Safety checks
    _assert_equal_size(out_equal, 1);
    _assert_equal_size(out_greater, 1);
    _assert_equal_size(out_smaller, 1);

    comparator(input1, input2, out_equal[0], out_greater[0], out_smaller[0]);
}
