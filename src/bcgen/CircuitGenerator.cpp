#include <bcgen/version.hpp>
#include <bcgen/CircuitGenerator.hpp>
#include <fmt/format.h>

void gabe::bcgen::CircuitGenerator::_create_save_directory() {
    // There is nothing to be created if the specified directory is empty
    if (_circuits_directory.empty()) return;

    // Create circuits directory
    std::filesystem::create_directory(_circuits_directory);
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

void gabe::bcgen::CircuitGenerator::_write_1_1_gate(const uint64_t in_a, const uint64_t output, const std::string &gate) {
    // Line construction
    const std::string line = fmt::format("1 1 {} {} {}\n", in_a, output, gate);
    _write_gate(line, gate);
}

void gabe::bcgen::CircuitGenerator::_write_2_1_gate(const uint64_t in_a, const uint64_t in_b, const uint64_t output, const std::string &gate) {
    // Line construction
    const std::string line = fmt::format("2 1 {} {} {} {}\n", in_a < in_b ? in_a : in_b, in_a < in_b ? in_b : in_a, output, gate);
    _write_gate(line, gate);
}

void gabe::bcgen::CircuitGenerator::_assert_equal_size(const Variable &variable, uint64_t size) {
    // Checks if the variable as the expected size
    if (variable.size() != size) {
        // Creates the error message
        const std::string error_msg = fmt::format("Variable should have a size of {}.", size);

        // Raises the error
        throw std::runtime_error(error_msg);
    }
}

void gabe::bcgen::CircuitGenerator::_assert_add_input(uint64_t size) {
    // Checks if there are enough available input wires to add the input
    if (_counter_wires + size > _expected_input_wires) {
        // Creates the error message
        const std::string error_msg = fmt::format("There aren't enough input wires available to add an input of size {}.", size);
        
        // Raises the error
        throw std::runtime_error(error_msg);
    }
}

gabe::bcgen::CircuitGenerator::CircuitGenerator(const std::string &circuit_name) : _circuit_name(circuit_name), _circuits_directory("circuits") {
    _create_save_directory();
}

gabe::bcgen::CircuitGenerator::CircuitGenerator(const std::string &circuit_name, const std::string &circuits_directory) : _circuit_name(circuit_name), _circuits_directory(circuits_directory) {
    _create_save_directory();
}

gabe::bcgen::CircuitGenerator::~CircuitGenerator() {
    std::remove((_circuits_directory / (_circuit_name + "_temp.txt")).c_str());
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
    }

    // Writes unflushed data inside the buffer 
    _flush_buffer(file);
}

void gabe::bcgen::CircuitGenerator::limit_buffer(uint64_t size) {
    _buffer_max_size = size;
}

void gabe::bcgen::CircuitGenerator::add_input_party(uint64_t size) {
    _input_parties.push_back(size);
    _expected_input_wires += size;
}

void gabe::bcgen::CircuitGenerator::add_output_party(uint64_t size) {
    _output_parties.push_back(size);
    _expected_output_wires += size;
}

void gabe::bcgen::CircuitGenerator::add_input(Wire& wire) {
    // Safety check
    _assert_add_input(1);
    
    // Assigns a label to the wire
    wire.label = _counter_wires++;
}

void gabe::bcgen::CircuitGenerator::add_input(Variable& variable) {
    // Safety check
    _assert_add_input(variable.size());

    // Assigns a label to all the wires
    for (int i = 0; i < variable.size(); i++) {
        variable[i].label = _counter_wires++;
    }
}

void gabe::bcgen::CircuitGenerator::start() {
    // Safety check - No input wires
    if (!_expected_input_wires) {
        const std::string error_msg = "There are no input wires defined.";
        throw std::runtime_error(error_msg);
    }

    // Safety check - Dead wires
    if (_counter_wires < _expected_input_wires) {
        const std::string error_msg = "Dead wires in the circuit. There are input wires that are not assigned to a variable.";
        throw std::runtime_error(error_msg);
    }

    // Creates the zero and one wires
    XOR( Wire(), Wire(), _zero_wire );
    INV( _zero_wire, _one_wire ); 
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
        throw std::runtime_error(error_msg);
    }

    // Writing phase
    _write_header(circuit);
    _write_circuit(circuit);

    // Closes the circuit file
    circuit.close();

    // Final output
    printf("\nSuccessfully created circuit %s.\n", _circuit_name.c_str());
    printf("> Total gates: %lu\n", _counter_gates);
    printf("-> OR: %lu\n", _gates_counters[_gates_map["or"]]);
    printf("-> XOR: %lu\n", _gates_counters[_gates_map["xor"]]);
    printf("-> AND: %lu\n", _gates_counters[_gates_map["and"]]);
    printf("-> INV: %lu\n", _gates_counters[_gates_map["inv"]]);
    printf("> Total wires: %lu\n", _counter_wires);
}

void gabe::bcgen::CircuitGenerator::assign_value(Wire& wire, uint8_t value) {
    // Assigns the zero or one wire label to the current wire
    wire = (value & 0x01) == 0 ? _zero_wire : _one_wire;
}

void gabe::bcgen::CircuitGenerator::assign_value(Variable& variable, uint64_t value) {
    // Assigns a wire label into each variable wire
    for (uint8_t i = 0; i < variable.size(); i++) {
        assign_value(variable[i], value >> i);
    }
}

void gabe::bcgen::CircuitGenerator::shift_left(Variable &variable, uint64_t amount) {
    // Shifting
    for (uint64_t i = variable.size(); i > amount; i--) {
        variable[i-1] = variable[i-1 - amount];
    }

    // Assign 0 to the new wires
    for (uint64_t i = 0; i < amount && i < variable.size(); i++) {
        variable[i] = _zero_wire;
    }
}

void gabe::bcgen::CircuitGenerator::shift_left(const Variable& variable, uint64_t amount, Variable& output) {
    // Makes a copy of the input variable
    output = variable;

    // Shift the output variable
    shift_left(output, amount);
}

void gabe::bcgen::CircuitGenerator::shift_right(Variable &variable, uint64_t amount) {
    // Shifting
    for (uint64_t i = variable.size(); i > amount; i--) {
        variable[i-1 - amount] = variable[i-1];
    }
    
    // Assign 0 to the new wires
    for (uint64_t i = 0; i < amount && i < variable.size(); i++) {
        variable[variable.size() - 1 - i] = _zero_wire;
    }
}

void gabe::bcgen::CircuitGenerator::shift_right(const Variable& variable, uint64_t amount, Variable& output) {
    // Makes a copy of the input variable
    output = variable;

    // Shift the output variable
    shift_right(output, amount);
}

void gabe::bcgen::CircuitGenerator::rotate_left(Variable &variable, uint64_t amount) {
    // Creates a copy of the input variable
    Variable temp = variable;

    // Places the bits in the new positions due to the rotation
    for (uint64_t i = 0; i < variable.size(); i++) {
        // New position
        uint64_t j = (i + amount) % variable.size();

        // Bit replacement
        variable[i] = temp[j];
    }
}

void gabe::bcgen::CircuitGenerator::rotate_left(const Variable& variable, uint64_t amount, Variable& output) {
    // Makes a copy of the input variable
    output = variable;

    // Rotate the output variable
    rotate_left(output, amount);
}

void gabe::bcgen::CircuitGenerator::rotate_right(Variable &variable, uint64_t amount) {
    // Creates a copy of the input variable
    Variable temp = variable;

    // Places the bits in the new positions due to the rotation
    for (uint64_t i = 0; i < variable.size(); i++) {
        // New position
        uint64_t j = (i + amount) % variable.size();

        // Bit replacement
        variable[j] = temp[i];
    }
}

void gabe::bcgen::CircuitGenerator::rotate_right(const Variable& variable, uint64_t amount, Variable& output) {
    // Makes a copy of the input variable
    output = variable;

    // Rotate the output variable
    rotate_right(output, amount);
}

void gabe::bcgen::CircuitGenerator::flip(Variable& variable) {
    // Creates a copy of the input variable
    Variable temp = variable;

    // Places the bits in the new positions due to the flipping
    for (uint64_t i = 0; i < variable.size(); i++) {
        // New position
        uint64_t j = variable.size() - 1 - i;

        // Bit replacement
        variable[i] = temp[j];
    }
}

void gabe::bcgen::CircuitGenerator::flip(const Variable& variable, Variable& output) {
    // Makes a copy of the input variable
    output = variable;

    // Flip the output variable
    flip(output);
}

void gabe::bcgen::CircuitGenerator::twos_complement(Variable& variable) {
    // Variable creations
    Variable one(variable.size());

    // Variable value initializations
    assign_value(one, 1);

    // Algorithm
    INV(variable, variable);
    sum(one, variable, variable);
}

void gabe::bcgen::CircuitGenerator::twos_complement(const Variable& variable, Variable& output) {
    // Makes a copy of the input variable
    output = variable;

    // Performs the complement to the output variable
    twos_complement(output);
}

void gabe::bcgen::CircuitGenerator::XOR(const Wire in_a, const Wire in_b, Wire& out) {
    _write_2_1_gate( in_a.label, in_b.label, out.label = _counter_wires++, _gates_map["xor"] );
}

void gabe::bcgen::CircuitGenerator::XOR(const Variable& in_a, const Variable& in_b, Variable& out) {
    // Safety checks
    _assert_equal_size(in_a, in_a.size());
    _assert_equal_size(in_b, in_b.size());

    // XOR all the wires
    for (int i = 0; i < out.size(); i++) {
        XOR(in_a[i], in_b[i], out[i]);
    }
}

void gabe::bcgen::CircuitGenerator::AND(const Wire in_a, const Wire in_b, Wire& out) {
    _write_2_1_gate( in_a.label, in_b.label, out.label = _counter_wires++, _gates_map["and"] );
}

void gabe::bcgen::CircuitGenerator::AND(const Variable& in_a, const Variable& in_b, Variable& out) {
    // Safety checks
    _assert_equal_size(in_a, out.size());
    _assert_equal_size(in_b, out.size());

    // AND all the wires
    for (int i = 0; i < out.size(); i++) {
        AND(in_a[i], in_b[i], out[i]);
    }
}

void gabe::bcgen::CircuitGenerator::OR(const Wire in_a, const Wire in_b, Wire& out) {
    _write_2_1_gate( in_a.label, in_b.label, out.label = _counter_wires++, _gates_map["or"] );
}

void gabe::bcgen::CircuitGenerator::OR(const Variable& in_a, const Variable& in_b, Variable& out) {
    // Safety checks
    _assert_equal_size(in_a, out.size());
    _assert_equal_size(in_b, out.size());

    // OR all the wires
    for (int i = 0; i < out.size(); i++) {
        OR(in_a[i], in_b[i], out[i]);
    }
}

void gabe::bcgen::CircuitGenerator::INV(const Wire in_a, Wire& out) {
    _write_1_1_gate( in_a.label, out.label = _counter_wires++, _gates_map["inv"] );
}

void gabe::bcgen::CircuitGenerator::INV(const Variable& in_a, Variable& out) {
    // Safety checks
    _assert_equal_size(in_a, out.size());

    // INV all the wires
    for (int i = 0; i < out.size(); i++) {
        INV(in_a[i], out[i]);
    }
}

void gabe::bcgen::CircuitGenerator::XNOR(const Wire in_a, const Wire in_b, Wire& out) {
    XOR(in_a, in_b, out);
    INV(out, out);
}

void gabe::bcgen::CircuitGenerator::XNOR(const Variable& in_a, const Variable& in_b, Variable& out) {
    XOR(in_a, in_b, out);
    INV(out, out);
}

void gabe::bcgen::CircuitGenerator::NAND(const Wire in_a, const Wire in_b, Wire& out) {
    AND(in_a, in_b, out);
    INV(out, out);
}

void gabe::bcgen::CircuitGenerator::NAND(const Variable& in_a, const Variable& in_b, Variable& out) {
    AND(in_a, in_b, out);
    INV(out, out);
}

void gabe::bcgen::CircuitGenerator::NOR(const Wire in_a, const Wire in_b, Wire& out) {
    OR(in_a, in_b, out);
    INV(out, out);
}

void gabe::bcgen::CircuitGenerator::NOR(const Variable& in_a, const Variable& in_b, Variable& out) {
    OR(in_a, in_b, out);
    INV(out, out);
}

void gabe::bcgen::CircuitGenerator::sum(const Variable& in_a, const Variable& in_b, Variable& out) {
#if BCGEN_OPTIMIZE
    // const Variable& biggest_in = in_a.size() < in_b.size() ? in_b : in_a;
    // const Variable& smallest_in = in_a.size() < in_b.size() ? in_a : in_b;

    // int n_operations = out_sum.size() < ;

    // assign_value(out_sum, 0);

    // -------
    
    // TODO: Think in the function optimization...
    // > Variables should have arbitrary size
    // > Perform as many bit operations as the length of the input variables

    // Examples
    // 
    // carry       11110    [5 bits]
    // in_a     01100011    [8 bits]
    // in_b         1111    [4 bits]
    // sum         10010    [5 bits]
    //
    // carry       11110    [5 bits]
    // in_a     01100011    [8 bits]
    // in_b        11111    [5 bits]
    // sum         00010    [5 bits]
    //
    // carry       11110    [5 bits]
    // in_a     01100011    [8 bits]
    // in_b       111111    [6 bits]
    // sum         00010    [5 bits]
    //
    // carry        1110    [4 bits]
    // in_a     01100011    [8 bits]
    // in_b          111    [3 bits]
    // sum         01010    [5 bits]
    //
    // The point here is to demonstrate the behavior changes when the output is 5 bits and the in_b varaibles have different sizes.
#else
    // Safety checks
    _assert_equal_size(in_a, out.size());
    _assert_equal_size(in_b, out.size());

    int n_operations = out.size();
#endif

    // Variables creation
    Wire c = _zero_wire; // This is the carry bit
    Variable a_xor_b (out.size()); // Consider as "d"
    Variable a_and_b (out.size());
    Variable c_and_d (out.size());

    // Circuit construction
    for (int i = 0; i < n_operations; i++) {
        // Sum wire
        XOR(in_a[i], in_b[i], a_xor_b[i]);
        XOR(a_xor_b[i], c, out[i]); // TODO: This must be placed somewhere else if output wires are aimed to be placed in the end

        // Carry out wire
        // > This operation is not done in the last bit to avoid dead wires in the circuit, or more gates to avoid it to be dead.
        if (i != n_operations - 1) {
            AND(in_a[i], in_b[i], a_and_b[i]);
            AND(c, a_xor_b[i], c_and_d[i]);
            OR(a_and_b[i], c_and_d[i], c);
        }
    }
}

void gabe::bcgen::CircuitGenerator::subtract(const Variable& in_a, const Variable& in_b, Variable& out) {
#if BCGEN_OPTIMIZE
    // TODO: Think in the function optimization...
    // > Variables should have arbitrary size
    // > Perform as many bit operations as the length of the input variables
#else
    // Safety checks
    _assert_equal_size(in_a, out.size());
    _assert_equal_size(in_b, out.size());

    int n_operations = out.size();
#endif

    // Variables creation
    Wire b = _zero_wire; // This is the borrow bit. Consider as "B"
    Variable a_xor_b (out.size());
    Variable b_xor_B (out.size()); // Consider as "c"
    Variable inv_a   (out.size()); // Consider as "d"
    Variable c_and_d (out.size());
    Variable b_and_B (out.size());

    // Circuit construction
    for (int i = 0; i < n_operations; i++) {
        // Sub wire
        XOR(in_a[i], in_b[i], a_xor_b[i]);
        XOR(a_xor_b[i], b, out[i]); // TODO: This must be placed somewhere else if output wires are aimed to be placed in the end

        // Borrow out wire
        // > This operation is not done in the last bit to avoid dead wires in the circuit, or more gates to avoid it to be dead.
        if (i != n_operations - 1) {
            XOR(in_b[i], b, b_xor_B[i]);
            INV(in_a[i], inv_a[i]);
            AND(b_xor_B[i], inv_a[i], c_and_d[i]);
            AND(in_b[i], b, b_and_B[i]);
            OR(c_and_d[i], b_and_B[i], b);
        }
    }
}

void gabe::bcgen::CircuitGenerator::multiply_u(const Variable &in_a, const Variable &in_b, Variable &out) {
#if BCGEN_OPTIMIZE
    // TODO: Think in the function optimization...
    // > Variables should have arbitrary size
    // > Perform as many bit operations as the length of the input variables
#else
    // Safety checks
    _assert_equal_size(in_a, in_b.size());
    _assert_equal_size(out, in_a.size() + in_b.size());

    // Variables creation
    for (int i = 0; i < in_a.size(); i++) {
        AND(in_a[i], in_b[0], out[i]);
    }

    // Circuit construction
    for (int op = 1; op < in_b.size(); op++) {
        Variable sum_with(out.size());

        for (int i = 0; i < in_a.size(); i++) {
            AND(in_a[i], in_b[op], sum_with[i + op]);
        }

        sum(out, sum_with, out);
    }
#endif
}

void gabe::bcgen::CircuitGenerator::multiply_s(const Variable &in_a, const Variable &in_b, Variable &out) {
#if BCGEN_OPTIMIZE
    // TODO: Think in the function optimization...
    // > Variables should have arbitrary size
    // > Perform as many bit operations as the length of the input variables
#else
    // Safety checks
    _assert_equal_size(in_a, in_b.size());
    _assert_equal_size(out, in_a.size() + in_b.size());

    // Variables creation
    for (int i = 0; i < in_a.size(); i++) {
        AND(in_a[i], in_b[0], out[i]);
    }
    INV(out[in_a.size()-1], out[in_a.size()-1]);

    // Circuit construction
    for (int op = 1; op < in_b.size(); op++) {
        Variable sum_with(out.size());

        if (op != in_b.size() - 1) {
            for (int i = 0; i < in_a.size(); i++) {
                AND(in_a[i], in_b[op], sum_with[i + op]);
            }
            INV(sum_with[in_a.size()-1 + op], sum_with[in_a.size()-1 + op]);
        } else {
            for (int i = 0; i < in_a.size(); i++) {
                AND(in_a[i], in_b[op], sum_with[i + op]);
                if (i != in_a.size() - 1) {
                    INV(sum_with[in_a.size()-1 + op], sum_with[in_a.size()-1 + op]);
                }
            }
        }

        sum(out, sum_with, out);
    }

    Variable last_sum(out.size());
    assign_value(last_sum, 9);
    shift_left(last_sum, in_b.size(), last_sum);

    sum(out, last_sum, out);
#endif
}

void gabe::bcgen::CircuitGenerator::divide_u(const Variable& in_a, const Variable& in_b, Variable& out_q, Variable& out_r) {
#if BCGEN_OPTIMIZE
    // TODO: Think in the function optimization...
    // > Variables should have arbitrary size
    // > Perform as many bit operations as the length of the input variables
#else
    // Safety checks
    _assert_equal_size(in_a, out_q.size());
    _assert_equal_size(in_b, out_q.size());
    _assert_equal_size(out_r, out_q.size());

    // Variable value initializations
    assign_value(out_r, 0);

    // Control wires
    Wire control;

    // Relevant variables for circuit contruction
    // > Zero is a variable that will be used to subtract
    Variable zero(in_a.size());
    assign_value(zero, 0);

    // Circuit construction
    for (int i = in_a.size()-1; i >= 0; i--) {
        // Updates the remainder value
        // > Shifts left by 1
        // > Adds the most significant (not assigned) bit from the dividend to the remainder's least significant bit
        shift_left(out_r, 1);
        out_r[0] = in_a[i];

        // Checks if the current remainder value can subtract the divisor value
        // > This can only be done if remainder value >= divisor value.
        // > The control variable from this check will also be the quotient bit value
        greater_or_equal(out_r, in_b, control);

        // Defines if the value to subtract from the remainder if zero of the divisor
        Variable subtractor(in_a.size());
        multiplexer(zero, in_b, control, subtractor);

        // Performs the subtraction
        subtract(out_r, subtractor, out_r); 

        // Updates the quotient value
        out_q[i] = control;
    }
#endif
}

void gabe::bcgen::CircuitGenerator::divide_u_quotient(const Variable& in_a, const Variable& in_b, Variable& out_q) {
#if BCGEN_OPTIMIZE
    // TODO: Think in the function optimization...
    // > Variables should have arbitrary size
    // > Perform as many bit operations as the length of the input variables
#else
    // Safety checks
    _assert_equal_size(in_a, out_q.size());
    _assert_equal_size(in_b, out_q.size());

    // Control wires
    Wire control;

    // Relevant variables for circuit contruction
    // > Zero is a variable that will be used to subtract
    Variable zero(in_a.size());
    assign_value(zero, 0);
    // > Remainder is the other resulting value, that this function ignores
    Variable remainder(in_a.size());
    assign_value(remainder, 0);

    // Circuit construction
    for (int i = in_a.size()-1; i >= 0; i--) {
        // Updates the remainder value
        // > Shifts left by 1
        // > Adds the most significant (not assigned) bit from the dividend to the remainder's least significant bit
        shift_left(remainder, 1);
        remainder[0] = in_a[i];

        // Checks if the current remainder value can subtract the divisor value
        // > This can only be done if remainder value >= divisor value.
        // > The control variable from this check will also be the quotient bit value
        greater_or_equal(remainder, in_b, control);

        // We do not want to perform the following operations in the last bit
        // > Otherwise the remainder wires would be left "dead"
        if (i != 0) {
            // Defines if the value to subtract from the remainder if zero of the divisor
            Variable subtractor(in_a.size());
            multiplexer(zero, in_b, control, subtractor);

            // Performs the subtraction
            subtract(remainder, subtractor, remainder);
        }

        // Updates the quotient value
        out_q[i] = control;
    }
#endif
}

void gabe::bcgen::CircuitGenerator::divide_u_remainder(const Variable& in_a, const Variable& in_b, Variable& out_r) {
#if BCGEN_OPTIMIZE
    // TODO: Think in the function optimization...
    // > Variables should have arbitrary size
    // > Perform as many bit operations as the length of the input variables
#else
    // Creates the quotient variable
    // > This is temporary. This function ignores this result
    Variable quotient(out_r.size());

    // Performs the division
    divide_u(in_a, in_b, quotient, out_r);
#endif
}

void gabe::bcgen::CircuitGenerator::divide_s(const Variable& in_a, const Variable& in_b, Variable& out_q, Variable& out_r) {
#if BCGEN_OPTIMIZE
    // TODO: Think in the function optimization...
    // > Variables should have arbitrary size
    // > Perform as many bit operations as the length of the input variables
#else
    // Safety checks
    _assert_equal_size(in_a, out_q.size());
    _assert_equal_size(in_b, out_q.size());
    _assert_equal_size(out_r, out_q.size());

    // Input variable signs
    // > This will define what will be the dividend and divisor variable of the division.
    // > If A sign is positive, in_a will be used, otherwise its two's complement
    // > If B sign is positive, in_b will be used, otherwise its two's complement
    Wire in_a_sign = in_a[in_a.size()-1];
    Wire in_b_sign = in_b[in_b.size()-1];

    // Inputs two's complements
    Variable in_a_complement(in_a.size());
    Variable in_b_complement(in_b.size());
    twos_complement(in_a, in_a_complement);
    twos_complement(in_b, in_b_complement);

    // Define the dividend and divisor variables that will be used
    Variable dividend(in_a.size());
    Variable divisor(in_b.size());
    multiplexer(in_a, in_a_complement, in_a_sign, dividend);
    multiplexer(in_b, in_b_complement, in_b_sign, divisor);

    // Performs the unsigned division
    divide_u(dividend, divisor, out_q, out_r);

    // Quotient variable sign
    // > This will control if the quotient result has to be manipulated with the two's complement
    Wire out_sign;
    XOR(in_a_sign, in_b_sign, out_sign);

    // Quotient two's complement
    Variable out_q_complement(out_q.size());
    twos_complement(out_q, out_q_complement);
    
    // Define the output quotient
    multiplexer(out_q, out_q_complement, out_sign, out_q);
#endif
}

void gabe::bcgen::CircuitGenerator::divide_s_quotient(const Variable& in_a, const Variable& in_b, Variable& out_q) {
#if BCGEN_OPTIMIZE
    // TODO: Think in the function optimization...
    // > Variables should have arbitrary size
    // > Perform as many bit operations as the length of the input variables
#else
    // Safety checks
    _assert_equal_size(in_a, out_q.size());
    _assert_equal_size(in_b, out_q.size());

    // Input variable signs
    // > This will define what will be the dividend and divisor variable of the division.
    // > If A sign is positive, in_a will be used, otherwise its two's complement
    // > If B sign is positive, in_b will be used, otherwise its two's complement
    Wire in_a_sign = in_a[in_a.size()-1];
    Wire in_b_sign = in_b[in_b.size()-1];

    // Inputs two's complements
    Variable in_a_complement(in_a.size());
    Variable in_b_complement(in_b.size());
    twos_complement(in_a, in_a_complement);
    twos_complement(in_b, in_b_complement);

    // Define the dividend and divisor variables that will be used
    Variable dividend(in_a.size());
    Variable divisor(in_b.size());
    multiplexer(in_a, in_a_complement, in_a_sign, dividend);
    multiplexer(in_b, in_b_complement, in_b_sign, divisor);

    // Performs the unsigned division
    divide_u_quotient(dividend, divisor, out_q);

    // Quotient variable sign
    // > This will control if the quotient result has to be manipulated with the two's complement
    Wire out_sign;
    XOR(in_a_sign, in_b_sign, out_sign);

    // Quotient two's complement
    Variable out_q_complement(out_q.size());
    twos_complement(out_q, out_q_complement);
    
    // Define the output quotient
    multiplexer(out_q, out_q_complement, out_sign, out_q);
#endif
}

void gabe::bcgen::CircuitGenerator::divide_s_remainder(const Variable& in_a, const Variable& in_b, Variable& out_r) {
#if BCGEN_OPTIMIZE
    // TODO: Think in the function optimization...
    // > Variables should have arbitrary size
    // > Perform as many bit operations as the length of the input variables
#else
    // Safety checks
    _assert_equal_size(in_a, out_r.size());
    _assert_equal_size(in_b, out_r.size());

    // Input variable signs
    // > This will define what will be the dividend and divisor variable of the division.
    // > If A sign is positive, in_a will be used, otherwise its two's complement
    // > If B sign is positive, in_b will be used, otherwise its two's complement
    Wire in_a_sign = in_a[in_a.size()-1];
    Wire in_b_sign = in_b[in_b.size()-1];

    // Inputs two's complements
    Variable in_a_complement(in_a.size());
    Variable in_b_complement(in_b.size());
    twos_complement(in_a, in_a_complement);
    twos_complement(in_b, in_b_complement);

    // Define the dividend and divisor variables that will be used
    Variable dividend(in_a.size());
    Variable divisor(in_b.size());
    multiplexer(in_a, in_a_complement, in_a_sign, dividend);
    multiplexer(in_b, in_b_complement, in_b_sign, divisor);

    // Performs the unsigned division
    divide_u_remainder(dividend, divisor, out_r);
#endif
}

void gabe::bcgen::CircuitGenerator::multiplexer(const Variable& in_a, const Variable& in_b, const Wire& in_c, Variable& out) {
    // Safety checks
    _assert_equal_size(in_a, in_b.size());

    // Variables creation
    Variable in_a_and_not_c(in_a.size());
    Variable in_b_and_c(in_b.size());

    // Negates the control bit
    Wire not_c;
    INV(in_c, not_c);

    // Circuit construction
    for (int i = 0; i < in_a.size(); i++) {
        // Decision wires
        Wire a_decision;
        Wire b_decision;

        // Current bit operations
        AND(in_a[i], not_c, a_decision);
        AND(in_b[i], in_c , b_decision);
        OR(a_decision, b_decision, out[i]);
    }
}

void gabe::bcgen::CircuitGenerator::equal(const Variable& in_a, const Variable& in_b, Wire& out) {
#if BCGEN_OPTIMIZE
    // TODO: Think in the function optimization...
    // > Variables should have arbitrary size
    // > Perform as many bit operations as the length of the input variables
#else
    // Safety checks
    _assert_equal_size(in_a, in_b.size());

    // XORs every wire between the variables
    Variable inputs_xor(in_a.size());
    XOR(in_a, in_b, inputs_xor);

    // Starts the output result with the first wire XOR
    out = inputs_xor[0];

    // ORs every single wire with the current output result
    for (int i = 1; i < inputs_xor.size(); i++) {
        OR(inputs_xor[i], out, out);
    }

    // Inverts the output result
    // > Until here we have that:
    //   > Output = 0 : Variables are equal.
    //   > Output = 1 : Variables are not equal.
    // > We want to output 1 if equal, 0 otherwise.
    INV(out, out);
#endif
}

void gabe::bcgen::CircuitGenerator::equal(const Variable& in_a, const Variable& in_b, Variable& out) {
    assign_value(out, 0);

    // Performs the equal operation
    equal(in_a, in_b, out[0]);
}

void gabe::bcgen::CircuitGenerator::greater(const Variable& in_a, const Variable& in_b, Wire& out) {
#if BCGEN_OPTIMIZE
    // TODO: Think in the function optimization...
    // > Variables should have arbitrary size
    // > Perform as many bit operations as the length of the input variables
#else
    // Safety checks
    _assert_equal_size(in_a, in_b.size());

    // Util variables
    const uint64_t msb = in_a.size() - 1; // msb = Most Significant Bit

    // Calculates A AND NOT_B
    // > All the B bits must be negated
    // > All the B negated bits must be AND with the A bits
    Variable a_and_not_b(in_b.size());
    {
        Variable not_b(in_b.size());
        INV(in_b, not_b);
        AND(in_a, not_b, a_and_not_b);
    }

    // Calculates all the XNORs
    // > All the bits need to be XNORed, except the least significant one
    // > The i-th xnored bit must be AND with the XNORs from all its most significant bits
    // > (Check documentation expression)
    Variable xnors(in_a.size());
    for (int i = msb; i > 0; i--) {
        // XNOR current bit
        XNOR(in_a[i], in_b[i], xnors[i]);

        // Updates the XNOR chain of the current bit
        if (i != msb) {
            AND(xnors[i], xnors[i+1], xnors[i]);
        }
    }

    // Assigns the most significant bit to the output result
    out = a_and_not_b[msb];

    // Calculates all the operations between the ORs
    // > -1 is because the most significant bit is already assigned in the output
    for (int i = 0; i < in_a.size() - 1; i++) {
        // Current operation between ORs
        Wire cur_operation;
        AND(a_and_not_b[i], xnors[i+1], cur_operation);

        // Updates the output
        OR(cur_operation, out, out);
    }
#endif
}

void gabe::bcgen::CircuitGenerator::greater(const Variable& in_a, const Variable& in_b, Variable& out) {
    assign_value(out, 0);

    // Performs the greater operation
    greater(in_a, in_b, out[0]);
}

void gabe::bcgen::CircuitGenerator::smaller(const Variable& in_a, const Variable& in_b, Wire& out) {
#if BCGEN_OPTIMIZE
    // TODO: Think in the function optimization...
    // > Variables should have arbitrary size
    // > Perform as many bit operations as the length of the input variables
#else
    // Safety checks
    _assert_equal_size(in_a, in_b.size());

    // Util variables
    const uint64_t msb = in_a.size() - 1; // msb = Most Significant Bit

    // Calculates NOT_A AND B
    // > All the A bits must be negated
    // > All the A negated bits must be AND with the B bits
    Variable not_a_and_b(in_a.size());
    {
        Variable not_a(in_a.size());
        INV(in_a, not_a);
        AND(not_a, in_b, not_a_and_b);
    }

    // Calculates all the XNORs
    // > All the bits need to be XNORed, except the least significant one
    // > The i-th xnored bit must be AND with the XNORs from all its most significant bits
    // > (Check documentation expression)
    Variable xnors(in_a.size());
    for (int i = msb; i > 0; i--) {
        // XNOR current bit
        XNOR(in_a[i], in_b[i], xnors[i]);

        // Updates the XNOR chain of the current bit
        if (i != msb) {
            AND(xnors[i], xnors[i+1], xnors[i]);
        }
    }

    // Assigns the most significant bit to the output result
    out = not_a_and_b[msb];

    // Calculates all the operations between the ORs
    // > -1 is because the most significant bit is already assigned in the output
    for (int i = 0; i < in_a.size() - 1; i++) {
        // Current operation between ORs
        Wire cur_operation;
        AND(not_a_and_b[i], xnors[i+1], cur_operation);

        // Updates the output
        OR(cur_operation, out, out);
    }
#endif
}

void gabe::bcgen::CircuitGenerator::smaller(const Variable& in_a, const Variable& in_b, Variable& out) {
    assign_value(out, 0);

    // Performs the smaller operation
    smaller(in_a, in_b, out[0]);
}

void gabe::bcgen::CircuitGenerator::greater_or_equal(const Variable& in_a, const Variable& in_b, Wire& out) {
    smaller(in_a, in_b, out);
    INV(out, out);
}

void gabe::bcgen::CircuitGenerator::greater_or_equal(const Variable& in_a, const Variable& in_b, Variable& out) {
    assign_value(out, 0);

    // Performs the greater or equal operation
    greater_or_equal(in_a, in_b, out[0]);
}

void gabe::bcgen::CircuitGenerator::smaller_or_equal(const Variable& in_a, const Variable& in_b, Wire& out) {
    greater(in_a, in_b, out);
    INV(out, out);
}

void gabe::bcgen::CircuitGenerator::smaller_or_equal(const Variable& in_a, const Variable& in_b, Variable& out) {
    assign_value(out, 0);

    // Performs the smaller or equal operation
    smaller_or_equal(in_a, in_b, out[0]);
}

void gabe::bcgen::CircuitGenerator::comparator(const Variable& in_a, const Variable& in_b, Wire& out_e, Wire& out_g, Wire& out_s) {
    // Safety checks
    _assert_equal_size(in_a, in_b.size());

    // Util variables
    const uint64_t msb = in_a.size() - 1; // msb = Most Significant Bit

    // Calculates A AND NOT_B (greater part)
    // > All the B bits must be negated
    // > All the B negated bits must be AND with the A bits
    Variable a_and_not_b(in_b.size());
    {
        Variable not_b(in_b.size());
        INV(in_b, not_b);
        AND(in_a, not_b, a_and_not_b);
    }

    // Calculates NOT_A AND B (smaller part)
    // > All the A bits must be negated
    // > All the A negated bits must be AND with the B bits
    Variable not_a_and_b(in_a.size());
    {
        Variable not_a(in_a.size());
        INV(in_a, not_a);
        AND(not_a, in_b, not_a_and_b);
    }

    // Calculates all the XNORs (all parts)
    // > All the bits need to be XNORed, except the least significant one
    // > The i-th xnored bit must be AND with the XNORs from all its most significant bits
    // > (Check documentation expression)
    Variable xnors(in_a.size());
    for (int i = msb; i > 0; i--) {
        // XNOR current bit
        XNOR(in_a[i], in_b[i], xnors[i]);

        // Updates the XNOR chain of the current bit
        if (i != msb) {
            AND(xnors[i], xnors[i+1], xnors[i]);
        }
    }

    // Assigns the most significant bit to the greater and smaller output results
    out_g = a_and_not_b[msb];
    out_s = not_a_and_b[msb];

    // Calculates all the operations between the ORs
    // > -1 is because the most significant bit is already assigned in the output
    for (int i = 0; i < in_a.size() - 1; i++) {
        // Current operation between ORs (greater part)
        Wire cur_operation_g;
        AND(a_and_not_b[i], xnors[i+1], cur_operation_g);

        // Current operation between ORs (smaller part)
        Wire cur_operation_s;
        AND(not_a_and_b[i], xnors[i+1], cur_operation_s);

        // Updates the outputs
        OR(cur_operation_g, out_g, out_g);
        OR(cur_operation_s, out_s, out_s);
    }

    // Finally, the equal part
    OR(out_g, out_s, out_e);
    INV(out_e, out_e);
}

void gabe::bcgen::CircuitGenerator::comparator(const Variable& in_a, const Variable& in_b, Variable& out_e, Variable& out_g, Variable &out_s) {
    assign_value(out_e, 0);
    assign_value(out_g, 0);
    assign_value(out_s, 0);

    // Performs the smaller or equal operation
    comparator(in_a, in_b, out_e[0], out_g[0], out_s[0]);
}
