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
    if (var1.size() != var2.size())
        throw std::invalid_argument("The inserted variables do not share the same size.");
}

void gabe::circuits::generator::CircuitGenerator::_assert_equal_size(const UnsignedVar& var1, const UnsignedVar& var2) {
    if (var1.size() != var2.size())
        throw std::invalid_argument("The inserted variables do not share the same size.");
}

void gabe::circuits::generator::CircuitGenerator::_assert_equal_size(const SignedVar& var, const uint64_t size) {
    if (var.size() != size)
        throw std::invalid_argument("The inserted variables do not share the same size.");
}

void gabe::circuits::generator::CircuitGenerator::_assert_equal_size(const UnsignedVar& var, const uint64_t size) {
    if (var.size() != size)
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

void gabe::circuits::generator::CircuitGenerator::assign_value(SignedVar& variable, int64_t value) {
    for (uint8_t i = 0; i < variable.size(); i++) {
        // Gets the current bit value
        uint8_t bit_value = (value >> i) & 0x01;

        // Assigns the zero or one wire label to the current wire
        variable[i].label = bit_value == 0 ? _zero_wire.label : _one_wire.label;
    }
}

void gabe::circuits::generator::CircuitGenerator::assign_value(UnsignedVar& variable, uint64_t value) {
    for (uint8_t i = 0; i < variable.size(); i++) {
        // Gets the current bit value
        uint8_t bit_value = (value >> i) & 0x01;

        // Assigns the zero or one wire label to the current wire
        variable[i].label = bit_value == 0 ? _zero_wire.label : _one_wire.label;
    }
}

void gabe::circuits::generator::CircuitGenerator::shift_left(SignedVar &variable, uint64_t amount) {
    // Shifting
    for (uint64_t i = variable.size() - 1; i >= amount; i--)
        variable[i] = variable[i - amount];
    
    // Assign 0 to the new wires
    for (uint64_t i = 0; i < amount && i < variable.size(); i++)
        variable[i] = _zero_wire;
}

void gabe::circuits::generator::CircuitGenerator::shift_left(UnsignedVar &variable, uint64_t amount) {
    // Shifting
    for (uint64_t i = variable.size() - 1; i >= amount; i--)
        variable[i] = variable[i - amount];
    
    // Assign 0 to the new wires
    for (uint64_t i = 0; i < amount && i < variable.size(); i++)
        variable[i] = _zero_wire;
}

void gabe::circuits::generator::CircuitGenerator::shift_right(SignedVar &variable, uint64_t amount) {
    // Shifting
    for (int64_t i = 0; i < (int64_t)variable.size() - (int64_t)amount; i++)
        variable[i] = variable[i + amount];
    
    // Assign 0 to the new wires
    for (int64_t i = variable.size() - 1i64; i > (int64_t)variable.size() - 1i64 - (int64_t)amount && i >= 0; i--)
        variable[i] = _zero_wire;
}

void gabe::circuits::generator::CircuitGenerator::shift_right(UnsignedVar &variable, uint64_t amount) {
    // Shifting
    for (int64_t i = 0; i < (int64_t)variable.size() - (int64_t)amount; i++)
        variable[i] = variable[i + amount];
    
    // Assign 0 to the new wires
    for (int64_t i = variable.size() - 1i64; i > (int64_t)variable.size() - 1i64 - (int64_t)amount && i >= 0; i--)
        variable[i] = _zero_wire;
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
    for (int i = 0; i < input1.size(); i++)
        xor(input1[i], input2[i], output[i]);
}

void gabe::circuits::generator::CircuitGenerator::and(const SignedVar& input1, const SignedVar& input2, SignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // AND all the wires
    for (int i = 0; i < input1.size(); i++)
        and(input1[i], input2[i], output[i]);
}

void gabe::circuits::generator::CircuitGenerator::inv(const SignedVar& input, SignedVar& output) {
    // Safety checks
    _assert_equal_size(input, output);

    // INV all the wires
    for (int i = 0; i < input.size(); i++)
        inv(input[i], output[i]);
}

void gabe::circuits::generator::CircuitGenerator::or(const SignedVar& input1, const SignedVar& input2, SignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // OR all the wires
    for (int i = 0; i < input1.size(); i++)
        or(input1[i], input2[i], output[i]);
}

void gabe::circuits::generator::CircuitGenerator::xor(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // XOR all the wires
    for (int i = 0; i < input1.size(); i++)
        xor(input1[i], input2[i], output[i]);
}

void gabe::circuits::generator::CircuitGenerator::and(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // AND all the wires
    for (int i = 0; i < input1.size(); i++)
        and(input1[i], input2[i], output[i]);
}

void gabe::circuits::generator::CircuitGenerator::inv(const UnsignedVar& input, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input, output);

    // INV all the wires
    for (int i = 0; i < input.size(); i++)
        inv(input[i], output[i]);
}

void gabe::circuits::generator::CircuitGenerator::or(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    // OR all the wires
    for (int i = 0; i < input1.size(); i++)
        or(input1[i], input2[i], output[i]);
}

void gabe::circuits::generator::CircuitGenerator::addition(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
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
        xor(input1[i], input2[i], xor_a_b[i]);

        // Do not perform these operations if it is the last cycle
        if (i != output.size() - 1) {
            and(input1[i], input2[i], and_a_b[i]);
            and(xor_a_b[i], carry[i], and_d_c[i]);
            or(and_a_b[i], and_d_c[i], carry[i + 1]);
        }
    }

    // Final operations (only done like this to put the output wires last in the writting phase)
    // TODO - Think of a solution to make this possible without having this separated for cycle
    for (int i = 0; i < output.size(); i++)
        xor(xor_a_b[i], carry[i], output[i]);
}

void gabe::circuits::generator::CircuitGenerator::subtraction(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
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
        xor(input1[i], input2[i], xor_a_b[i]);

        // Do not perform these operations if it is the last cycle
        if (i != output.size() - 1) {
            inv(xor_a_b[i], inv_d[i]);
            inv(input1[i], inv_a[i]);
            and(inv_d[i], carry[i], and_d_c[i]);
            and(inv_a[i], input2[i], and_a_c[i]);
            or(and_d_c[i], and_a_c[i], carry[i]);
        }
    }

    // Final operations (only done like this to put the output wires last in the writting phase)
    // TODO - Think of a solution to make this possible without having this separated for cycle
    for (int i = 0; i < output.size(); i++)
        xor(xor_a_b[i], carry[i], output[i]);
}

void gabe::circuits::generator::CircuitGenerator::multiplication(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    // TODO - Think of any safety checks

    // Variables for multiplication calculation
    std::vector<UnsignedVar> variables(input2.size(), UnsignedVar(output.size())); // As many as the size of the input2 variable

    // Variables creation
    for (int i = 0; i < input2.size(); i++) {
        assign_value(variables.at(i), 0);

        // Variable is 0 or equal to input2 (shifted j, where j is the iteration step)
        for (int j = 0; j < input1.size(); j++)
            and(input1[j], input2[i], variables.at(i)[i+j]);
    }

    // Final operations
    for (int i = 1; i < variables.size(); i++) {
        if (i == 1)
            addition(variables.at(i - 1), variables.at(i), output);
        else
            addition(output, variables.at(i), output);
    }
}

void gabe::circuits::generator::CircuitGenerator::division(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output_quotient, UnsignedVar& output_remainder) {
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

void gabe::circuits::generator::CircuitGenerator::multiplexer(const Wire& control, const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(input1, input2);
    _assert_equal_size(input1, output);

    Wire not_control;
    inv(control, not_control);

    UnsignedVar and_in1(input1.size());
    UnsignedVar and_in2(input2.size());

    for (int i = 0; i < input1.size(); i++) {
        and(not_control, input1[i], and_in1[i]);
        and(control, input2[i], and_in2[i]);
    }

    // Final operation (only done like this to put the output wires last in the writting phase)
    // TODO - Think of a solution to make this possible without having this separated for cycle
    for (int i = 0; i < input1.size(); i++)
        or(and_in1[i], and_in2[i], output[i]);
}

void gabe::circuits::generator::CircuitGenerator::multiplexer(const UnsignedVar& control, const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(control, 1);

    multiplexer(control[0], input1, input2, output);
}

void gabe::circuits::generator::CircuitGenerator::equal(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output) {
    // Safety checks
    _assert_equal_size(input1, input2);

    UnsignedVar inputs_xor(input1.size());
    xor(input1, input2, inputs_xor);

    // Already adds the first wire from the xor to the output
    output = inputs_xor[0];

    // ORs every single wire (if more than 1)
    for (int i = 1; i < inputs_xor.size(); i++)
        or(inputs_xor[i], output, output);

    // Inverts the output result (Until here the output is 1 every time wires i-th from the inputs differ)
    inv(output, output);
}

void gabe::circuits::generator::CircuitGenerator::equal(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(output, 1);

    equal(input1, input1, output[0]);
}

void gabe::circuits::generator::CircuitGenerator::greater(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output) {
    // TODO - Make a schematic in docs for this function, so people understand the following lines

    // Safety checks
    _assert_equal_size(input1, input2);

    UnsignedVar not_input2(input2.size());
    inv(input2, not_input2);

    UnsignedVar inputs_xnor(input1.size());
    // This is done like this instead of a simple xor and inv to the whole variables because the least significant wire from the inputs is not relevant for the xnor
    for (int i = 1; i < input1.size(); i++) {
        xor(input1[i], input2[i], inputs_xnor[i]);
        inv(inputs_xnor[i], inputs_xnor[i]);
    }

    // Performs the operations between the ORs (see schematic)
    UnsignedVar middle_operations(input1.size());
    for (int i = 0; i < middle_operations.size(); i++) {
        for (int j = i; j < middle_operations.size(); j++) {
            if (i == j)
                and(input1[j], not_input2[i], middle_operations[i]);
            else
                and(middle_operations[i], inputs_xnor[j], middle_operations[i]);
        }
    }

    // Already adds the first wire from the middle operations to the output
    output = middle_operations[0];

    // Perfoms the final operations - ORs every single wire (if more than 1)
    for (int i = 1; i < middle_operations.size(); i++)
        or(middle_operations[i], output, output);
}

void gabe::circuits::generator::CircuitGenerator::greater(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(output, 1);

    greater(input1, input2, output[0]);
}

void gabe::circuits::generator::CircuitGenerator::smaller(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output) {
    // TODO - Make a schematic in docs for this function, so people understand the following lines

    // Safety checks
    _assert_equal_size(input1, input2);

    UnsignedVar not_input1(input1.size());
    inv(input1, not_input1);

    UnsignedVar inputs_xnor(input1.size());
    // This is done like this instead of a simple xor and inv to the whole variables because the least significant wire from the inputs is not relevant for the xnor
    for (int i = 1; i < input1.size(); i++) {
        xor(input1[i], input2[i], inputs_xnor[i]);
        inv(inputs_xnor[i], inputs_xnor[i]);
    }

    // Performs the operations between the ORs (see schematic)
    UnsignedVar middle_operations(input1.size());
    for (int i = 0; i < middle_operations.size(); i++) {
        for (int j = i; j < middle_operations.size(); j++) {
            if (i == j)
                and(not_input1[j], input2[i], middle_operations[i]);
            else
                and(middle_operations[i], inputs_xnor[j], middle_operations[i]);
        }
    }

    // Already adds the first wire from the middle operations to the output
    output = middle_operations[0];

    // Perfoms the final operations - ORs every single wire (if more than 1)
    for (int i = 1; i < middle_operations.size(); i++)
        or(middle_operations[i], output, output);
}

void gabe::circuits::generator::CircuitGenerator::smaller(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(output, 1);

    smaller(input1, input2, output[0]);
}

void gabe::circuits::generator::CircuitGenerator::greater_or_equal(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output) {
    // Safety checks
    _assert_equal_size(input1, input2);

    smaller(input1, input2, output);
    inv(output, output);
}

void gabe::circuits::generator::CircuitGenerator::greater_or_equal(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(output, 1);

    greater_or_equal(input1, input2, output[0]);
}

void gabe::circuits::generator::CircuitGenerator::smaller_or_equal(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output) {
    // Safety checks
    _assert_equal_size(input1, input2);

    greater(input1, input2, output);
    inv(output, output);
}

void gabe::circuits::generator::CircuitGenerator::smaller_or_equal(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output) {
    // Safety checks
    _assert_equal_size(output, 1);

    smaller_or_equal(input1, input2, output[0]);
}

void gabe::circuits::generator::CircuitGenerator::comparator(const UnsignedVar& input1, const UnsignedVar& input2, Wire& out_equal, Wire& out_greater, Wire &out_smaller) {
    // Safety checks
    _assert_equal_size(input1, input2);

    // Performs the initial operations
    UnsignedVar not_input1(input1.size());
    UnsignedVar not_input2(input2.size());
    inv(input1, not_input1);
    inv(input2, not_input2);

    UnsignedVar inputs_xnor(input1.size());
    // This is done like this instead of a simple xor and inv to the whole variables because the least significant wire from the inputs is not relevant for the xnor
    for (int i = 1; i < input1.size(); i++) {
        xor(input1[i], input2[i], inputs_xnor[i]);
        inv(inputs_xnor[i], inputs_xnor[i]);
    }

    // Performs the operations between the ORs for both greater and smaller cases (see schematics)
    UnsignedVar middle_operations_greater(input1.size());
    UnsignedVar middle_operations_smaller(input1.size());
    for (int i = 0; i < middle_operations_greater.size(); i++) {
        for (int j = i; j < middle_operations_greater.size(); j++) {
            if (i == j) {
                and(input1[j], not_input2[i], middle_operations_greater[i]);
                and(not_input1[j], input2[i], middle_operations_smaller[i]);
            }
            else {
                and(middle_operations_greater[i], inputs_xnor[j], middle_operations_greater[i]);
                and(middle_operations_smaller[i], inputs_xnor[j], middle_operations_smaller[i]);
            }
        }
    }

    // Already adds the first wire from the middle operations to each output
    out_greater = middle_operations_greater[0];
    out_smaller = middle_operations_smaller[0];

    // Perfoms the final operations - ORs every single wire (if more than 1)
    for (int i = 1; i < middle_operations_greater.size(); i++) {
        or(middle_operations_greater[i], out_greater, out_greater);
        or(middle_operations_smaller[i], out_smaller, out_smaller);
    }

    or(out_greater, out_smaller, out_equal);
    inv(out_equal, out_equal);

    // TODO - I can't make this function have the very last wires as out_greater, out_smaller, and out_equal, as it requires the OR operation of out_greater and out_smaller to get the out_equal.
}

void gabe::circuits::generator::CircuitGenerator::comparator(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& out_equal, UnsignedVar& out_greater, UnsignedVar &out_smaller) {
    // Safety checks
    _assert_equal_size(out_equal, 1);
    _assert_equal_size(out_greater, 1);
    _assert_equal_size(out_smaller, 1);

    comparator(input1, input2, out_equal[0], out_greater[0], out_smaller[0]);
}
