#include <bcgen/CircuitGenerator.hpp>
#include <catch2/catch_test_macros.hpp>
#include <limits>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <fstream>

// Don't use this :)
using Wire = gabe::bcgen::Wire;
using Var = gabe::bcgen::Variable;
using CircuitGenerator = gabe::bcgen::CircuitGenerator;

class CircuitGeneratorTester : public CircuitGenerator
{
public:
    // Circuit name and location
    using CircuitGenerator::_circuit_name;
    using CircuitGenerator::_circuits_directory;

    // Circuit info
    using CircuitGenerator::_input_parties;
    using CircuitGenerator::_output_parties;
    using CircuitGenerator::_output_wires;
    using CircuitGenerator::_gates_map;

    // Circuit info complement - Control variables
    using CircuitGenerator::_counter_wires;
    using CircuitGenerator::_counter_gates;
    using CircuitGenerator::_expected_input_wires;
    using CircuitGenerator::_expected_output_wires;
    using CircuitGenerator::_gates_counters;

    // Circuit buffer - Memory management
    using CircuitGenerator::_buffer_size;
    using CircuitGenerator::_buffer_max_size;
    using CircuitGenerator::_buffer;

    // Zero and One wires
    using CircuitGenerator::_zero_wire;
    using CircuitGenerator::_one_wire;

public:
    CircuitGeneratorTester(const std::string &circuit_name) : CircuitGenerator(circuit_name) {}
    CircuitGeneratorTester(const std::string &circuit_name, const std::string &circuits_directory) : CircuitGenerator(circuit_name, circuits_directory) {}
};

TEST_CASE("Constructor") {
    // > SECTION - Test suit "Constructor"
    //
    // This test suit aims to test creation of a Circuit Generator object.
    // The Circuit Generator object only contains a single constructor. However, it can be used in two different ways since one of
    // the arguments has a default value.
    //
    // Note: The constructor of CircuitGenerator does not have a public visibility, thus an instance cannot be created.
    // For the sake of the test, the tester class makes it accessible.
    //
    // > Test suit index:
    // > Test 1: Only specify circuit name
    // > Test 2: Specify both circuit name and location

    // -------------------
    // > SECTION - Test 1: Only specify circuit name.
    // -------------------
    // This test constructors a CircuitGenerator object specifying the name of the circuit.
    // In this test, the following should be expected:
    // 1. Circuit name should be equal to the constructor input
    // 2. The circuit location should remain with the default value of "circuits"

    // Constructor 1
    CircuitGeneratorTester circuit1("Constructor_1");

    // Checks - Circuit name and location
    REQUIRE(circuit1._circuit_name == "Constructor_1");
    REQUIRE(circuit1._circuits_directory == "circuits");
    // > !SECTION - Test 1: Only specify circuit name.

    // -------------------
    // > SECTION - Test 2: Specify both circuit name and location.
    // -------------------
    // This test constructors a CircuitGenerator object specifying the name of the circuit and its location.
    // In this test, the following should be expected:
    // 1. Circuit name and location should be equal to the constructor inputs

    // Constructor 2
    CircuitGeneratorTester circuit2("Constructor_2", "circuits/tests");

    // Checks - Circuit name and location
    REQUIRE(circuit2._circuit_name == "Constructor_2");
    REQUIRE(circuit2._circuits_directory == "circuits/tests");
    // > !SECTION - Test 2: Specify both circuit name and location.
    // > !SECTION - Test suit "Constructor"
}

TEST_CASE("Circuit Setup") {
    // > SECTION - Test suit "Circuit Setup"
    //
    // This test suit aims to test all the base functionalities of the Circuit Generator class.
    // It also tests the correct and incorrect orders to call these base functionalities.
    //
    // Note: The constructor of CircuitGenerator does not have a public visibility, thus an instance cannot be created.
    // For the sake of the tests, the tester class makes it accessible.
    //
    // Test suit index:
    // > Test 1: Start circuit writing without any input party
    // > Test 2: Add input parties
    // > Test 3: Start circuit writing without input wires assigned
    // > Test 4: Add input variables
    // > Test 5: Start circuit writing without any output party
    // > Test 6: Add output parties
    // > Test 7: Start circuit writing without output wires assigned
    // > Test 8: Add output variables
    // > Test 9: Assign input variable with more wires than available
    // > Test 10: Assign output variable with more wires than available
    // > Test 11: Start circuit with all input and outputs wires assigned
    // > Test 12: Stop circuit writing

    // -------------------
    // > SECTION - Setup
    // -------------------

    // Circuit Generator constructor
    CircuitGeneratorTester circuit("Core", "circuits/tests");
    // > !SECTION - Setup

    // -------------------
    // > SECTION - Test 1: Start circuit writing without any input party.
    // -------------------
    // This test executes the start() method right after the creation of the circuit generator object. A runtime error exception
    // should be raised, not allowing the start of the circuit writing.
    //
    // A circuit can only be wrote if:
    // 1. There is at least an input party defined
    // 2. There is at least an output party defined
    // 3. All the reserved input wires are assigned to inputs.
    // 4. All the reserved output wires are assigned to outputs.

    // Control variable
    bool failed_t1 = false;

    // Starts the circuit writing. Control variable should change.
    try { circuit.start(); }
    catch (std::runtime_error) { failed_t1 = true; }

    // Check - A runtime error has to be raised
    REQUIRE(failed_t1);
    // > !SECTION - Test 1: Start circuit writing without any input party.

    // -------------------
    // > SECTION - Test 2: Add input parties.
    // -------------------
    // This test validates the execution of the add_input_party() method. This method should specify how many input wires are
    // defined for each input party. This test creates 5 input parties, each with 8 reserved input wires.
    //
    // With the usage of this method the following should be expected:
    // 1. class member "_input_parties" should increase its size by 1 for each add_input_party function call;
    // 2. each new element in the class member "_input_parties" should contain the number of input wires dedicated for the party,
    // given as input in the add_input_party function;
    // 3. class member "_expected_input_wires" (control variable) should increase its value by the size of each number of input
    // wires given on each add_input_party function call.

    // Constants
    const int TOTAL_INPUT_PARTIES = 5;
    const int INPUT_PARTY_SIZE = 8;
    
    for (int i = 1; i <= TOTAL_INPUT_PARTIES; i++) {
        // Adding the party
        circuit.add_input_party(INPUT_PARTY_SIZE);

        // Checks - Target class members that should change with add_input_party usage
        REQUIRE(circuit._input_parties.size() == i);
        REQUIRE(circuit._input_parties[i-1] == INPUT_PARTY_SIZE);
        REQUIRE(circuit._expected_input_wires == INPUT_PARTY_SIZE * i);
    }
    // > !SECTION - Test 2: Add input parties.

    // --------
    // > SECTION - Test 3: Start circuit writing without input wires assigned.
    // --------
    // This test executes the start() method right after all the input parties are defined. A runtime error exception should be
    // raised, not allowing the start of the circuit writing.
    // 
    // As also stated in Test 1, circuit can only be wrote if:
    // 1. There is at least an input party defined
    // 2. There is at least an output party defined
    // 3. All the reserved input wires are assigned to inputs.
    // 4. All the reserved output wires are assigned to outputs.

    // Control variable
    bool failed_t3 = false;

    // Starts the circuit writing. Control variable should change.
    try { circuit.start(); }
    catch (std::runtime_error) { failed_t3 = true; }

    // Check - A runtime error has to be raised
    REQUIRE(failed_t3);
    // > !SECTION - Test 3: Start circuit writing without input wires assigned.

    // --------
    // > SECTION - Test 4: Add input variables.
    // --------
    // This test validates the execution of the add_input() method. This method accepts wires/variables as inputs, and should
    // label the wires labels accordingly to the available input wires reserved by add_input_party() method.
    // This test creates a wire, a variable with 3 wires, and a variable with 4 wires (total 8 wires) per input party.
    //
    // With the usage of this method the following should be expected:
    // 1. class member "_counter_wires" should increase its size by the number of wires inputted;
    // 2. each inputted wire should be labeled in ascending order, from the least significant bit to the most significant bit.

    for (int i = 1; i <= TOTAL_INPUT_PARTIES; i++) {
        // Wires and variables to be inputted
        Wire wire;
        Var var1(3);
        Var var2(4);

        // Methods usage
        circuit.add_input(wire);
        circuit.add_input(var1);
        circuit.add_input(var2);

        // Checks - Target class members that should change with add_input usage
        REQUIRE(circuit._counter_wires == INPUT_PARTY_SIZE * i);

        // Checks - Wires labels
        REQUIRE(   wire.label == 0 + (i - 1) * INPUT_PARTY_SIZE);
        REQUIRE(var1[0].label == 1 + (i - 1) * INPUT_PARTY_SIZE);
        REQUIRE(var1[1].label == 2 + (i - 1) * INPUT_PARTY_SIZE);
        REQUIRE(var1[2].label == 3 + (i - 1) * INPUT_PARTY_SIZE);
        REQUIRE(var2[0].label == 4 + (i - 1) * INPUT_PARTY_SIZE);
        REQUIRE(var2[1].label == 5 + (i - 1) * INPUT_PARTY_SIZE);
        REQUIRE(var2[2].label == 6 + (i - 1) * INPUT_PARTY_SIZE);
        REQUIRE(var2[3].label == 7 + (i - 1) * INPUT_PARTY_SIZE);
    }
    // > !SECTION - Test 4: Add input variables.

    // --------
    // > SECTION - Test 5: Start circuit writing without any output party.
    // --------
    // This test executes the start() method right after all the input parties and input wires are defined. A runtime error
    // exception should be raised, not allowing the start of the circuit writing.
    // 
    // As also stated in Test 1, circuit can only be wrote if:
    // 1. There is at least an input party defined
    // 2. There is at least an output party defined
    // 3. All the reserved input wires are assigned to inputs.
    // 4. All the reserved output wires are assigned to outputs.

    // Control variable
    bool failed_t5 = false;

    // Starts the circuit writing. Control variable should change.
    try { circuit.start(); }
    catch (std::runtime_error) { failed_t5 = true; }

    // Check - A runtime error has to be raised
    REQUIRE(failed_t5);
    // > !SECTION - Test 5: Start circuit writing without any output party.

    // -------------------
    // > SECTION - Test 6: Add output parties.
    // -------------------
    // This test validates the execution of the add_output_party() method. This method should specify how many output wires are
    // defined for each output party. This test creates 2 output parties, each with 16 reserved output wires.
    // 
    // With the usage of this method the following should be expected:
    // 1. class member "_output_parties" should increase its size by 1 for each add_output_party function call;
    // 2. each new element in the class member "_output_parties" should contain the number of output wires dedicated for the
    // party, given as input in the add_output_party function;
    // 3. class member "_expected_output_wires" (control variable) should increase its value by the size of each number of output
    // wires given on each add_output_party function call.

    // Constants
    const int TOTAL_OUTPUT_PARTIES = 2;
    const int OUTPUT_PARTY_SIZE = 16;

    for (int i = 1; i <= TOTAL_OUTPUT_PARTIES; i++) {
        // Adding the party
        circuit.add_output_party(OUTPUT_PARTY_SIZE);

        // Checks - Target class members that should change with add_output_party usage
        REQUIRE(circuit._output_parties.size() == i);
        REQUIRE(circuit._output_parties[i-1] == OUTPUT_PARTY_SIZE);
        REQUIRE(circuit._expected_output_wires == OUTPUT_PARTY_SIZE * i);
    }
    // > !SECTION - Test 6: Add output parties.

    // --------
    // > SECTION - Test 7: Start circuit writing without output wires assigned.
    // --------
    // This test executes the start() method right after all the input parties, input wires, and output parties are defined. A
    // runtime error exception should be raised, not allowing the start of the circuit writing.
    // 
    // As also stated in Test 1, circuit can only be wrote if:
    // 1. There is at least an input party defined
    // 2. There is at least an output party defined
    // 3. All the reserved input wires are assigned to inputs.
    // 4. All the reserved output wires are assigned to outputs.

    // Control variable
    bool failed_t7 = false;

    // Starts the circuit writing. Control variable should change.
    try { circuit.start(); }
    catch (std::runtime_error) { failed_t7 = true; }

    // Check - A runtime error has to be raised
    REQUIRE(failed_t7);
    // > !SECTION - Test 7: Start circuit writing without output wires assigned.

    // --------
    // > SECTION - Test 8: Add output variables.
    // --------
    // This test validates the execution of the add_output() method. This method accepts wires/variables as inputs, and should
    // cache a pointer of the wires objects in a circuit generator class member. The amount of wires that can be given to this
    // method are controlled by the amount of wires reserved for outputs, given with the usage of the add_output_party() method.
    // This test creates a wire, a variable with 7 wires, and a variable with 8 wires (total 16 wires) per output party.
    //
    // With the usage of this method the following should be expected:
    // 1. class member "_assigned_output_wires" should increase its size by the number of wires inputted;
    // 2. each inputted wire should be labeled in ascending order, from the least significant bit to the most significant bit.

    // The variables have to be here, due to memory limitations
    Wire o_wire;
    Var o_var1(7);
    Var o_var2(8);

    for (int i = 1; i <= TOTAL_OUTPUT_PARTIES; i++) {
        // Wires and variables to be inputted

        // Methods usage
        circuit.add_output(o_wire);
        circuit.add_output(o_var1);
        circuit.add_output(o_var2);

        // Checks - Target class members that should change with add_output usage
        REQUIRE(circuit._output_wires.size() == OUTPUT_PARTY_SIZE * i);
    }
    // > !SECTION - Test 8: Add output variables.

    // --------
    // > SECTION - Test 9: Assign input variable with more wires than available.
    // --------
    // This test validates the execution of the add_input() method when there are not enough input wires available to assign.
    // This test creates a variable with 1 wire.
    // The runtime error exception should be expected to be raised.

    // Control variable
    bool failed_t9 = false;

    // Adds the variable into the circuit. Control variable should change.
    try {
        Var var(1);
        circuit.add_input(var);
    }
    catch (std::runtime_error) { failed_t9 = true; }

    // Check - A runtime error has to be raised
    REQUIRE(failed_t9);
    // > !SECTION - Test 9: Assign input variable with more wires than available.

    // --------
    // > SECTION - Test 10: Assign output variable with more wires than available.
    // --------
    // This test validates the execution of the add_output() method when there are not enough output wires available to assign.
    // This test creates a variable with 1 wire.
    // The runtime error exception should be expected to be raised.

    // Control variable
    bool failed_t10 = false;

    // Adds the variable into the circuit. Control variable should change.
    try {
        Var var(1);
        circuit.add_output(var);
    }
    catch (std::runtime_error) { failed_t10 = true; }

    // Check - A runtime error has to be raised
    REQUIRE(failed_t10);
    // > !SECTION - Test 10: Assign output variable with more wires than available.

    // --------
    // > SECTION - Test 11: Start circuit with all input wires assigned.
    // --------
    // This test executes the start() method in optimal conditions, i.e after defining all the input parties, and after assigning
    // all input wires into input variables.
    //
    // In these conditions, the following should be expected:
    // 1. class member "_zero_wire" should be labelled with the output of a XOR operation (operation not relevant now, just the
    // assigned label);
    // 2. class member "_one_wire" should be labelled with the output of a INV operation (operation not relevant now, just the
    // assigned label);
    // 3. class member "_counter_gates" should have a size of 2, from the definition of 1. and 2.;
    // 4. class member "_buffer" should also have a size of 2, with the two gate lines from 1. and 2.;
    // 5. the first element in the class member "_buffer" should contain the gate definition of the 1. operation:
    //      5.1 gate with 2 input wires;
    //      5.2 gate with 1 output wire;
    //      5.3 input wire 1 with label 0;
    //      5.4 input wire 2 with label 0;
    //      5.5 output wire with label X, where X is the number or inputted wires + 1;
    // 6. the second element in the class member "_buffer" should contain the gate definition of the 2. operation:
    //      6.1 gate with 1 input wires;
    //      6.2 gate with 1 output wire;
    //      6.3 input wire 1 with label X (zero wire);
    //      6.4 output wire with label X+1;
    // 7. class member "_buffer_size" value should be the sum of the two lines defined in 5. and 6.

    // Control variable
    bool failed_t11 = false;

    // Starts the circuit writing. Control variable should not change.
    try { circuit.start(); }
    catch (std::runtime_error) { failed_t11 = true; }

    // Check - A runtime error should not be raised
    REQUIRE_FALSE(failed_t11);

    // Check - Constant wires in the circuit
    REQUIRE(circuit._zero_wire.label == 40);
    REQUIRE(circuit._one_wire.label  == 41);

    // Check - Gate lines written
    REQUIRE(circuit._counter_gates == 2);
    REQUIRE(circuit._buffer.size() == 2);
    REQUIRE(circuit._buffer[0] == "2 1 0 0 40 \n"); // Wire 0
    REQUIRE(circuit._buffer[1] == "1 1 40 41 \n" ); // Wire 1
    REQUIRE(circuit._buffer_size == std::string("2 1 0 0 40 \n").size() + std::string("1 1 40 41 \n").size());
    // > !SECTION - Test 11: Start circuit with all input wires assigned.

    // --------
    // > SECTION - Test 12: Stop circuit writing.
    // --------
    // This test executes the stop() to conclude the writing of the circuit.
    // 
    // With the usage of this method, the following should be expected:
    // 1. No runtime error should be raised;
    // 2. Temporary circuit file not created;
    // 3. Circuit file created;
    // 4. Circuit file should have 34 lines written:
    //      4.1 First line is the zero_wire operation;
    //      4.2 Second line is the one_wire operation;
    //      4.3 All other lines should be the output wires with an AND operation (this ensures that the last written wires are the
    //          outputs).

    // Control variable
    bool failed_t12 = false;

    // Stops the circuit writing. Control variable should not change.
    try { circuit.stop(); }
    catch (std::runtime_error) { failed_t12 = true; }

    // Check - A runtime error should not be raised
    REQUIRE_FALSE(failed_t12);

    // Check - A temporary circuit file should have not been created
    REQUIRE_FALSE(std::filesystem::exists(circuit._circuits_directory / (circuit._circuit_name + "_temp.txt")));

    // Check - The circuit file should have been created
    REQUIRE(std::filesystem::exists(circuit._circuits_directory / (circuit._circuit_name + ".txt")));

    // Check - The circuit file should not contain any header section, only the two gate lines
    int lines_counter = 0;
    {
        std::ifstream file (circuit._circuits_directory / (circuit._circuit_name + ".txt"), std::ios::in);
        std::string line;
        while (std::getline(file, line)) { lines_counter++; }
        file.close();
    }
    REQUIRE(lines_counter == 34);
    // > !SECTION - Test 12: Stop circuit writing.
    // > !SECTION - Test suit "Circuit Setup"
}

TEST_CASE("Assign Value Operations") {
    // > SECTION - Test suit "Assign Value Operations"
    //
    // This test suit aims to test the assign value operation of the Circuit Generator class.
    // The assign value function can be used either to assign a value to a wire (which can assume a value of 0 or 1), or to
    // assign a value to a variable (which values are dependent on the size of the variable). This test suit will make sure that
    // the assign value function is tested with positive and negative numbers, as well as numbers that do and no not overflow.
    //
    // Note: The constructor of CircuitGenerator does not have a public visibility, thus an instance cannot be created.
    // For the sake of the tests, the tester class makes it accessible.
    //
    // Test suit index:
    // > Test 1: Assign values to a wire
    // > Test 2: Assign values to a variable

    // -------------------
    // > SECTION - Setup
    // -------------------
    // This section creates a circuit generator and prepares a circuit with 2 input parties, both with 8 dedicated input wires,
    // and a single output party, also of 8 wires.
    // The methods used in this setup already have tests covering them in Test suit "Core".

    // Circuit Generator constructor
    CircuitGeneratorTester circuit("AssignValueOperations", "circuits/tests");

    // Parties
    circuit.add_input_party(8);
    circuit.add_input_party(8);
    circuit.add_output_party(8);

    // Inputs
    Var in_1(8), in_2(8);
    circuit.add_input(in_1);
    circuit.add_input(in_2);

    // Outputs
    Var out(8);
    circuit.add_output(out);
    
    // Starts the circuit (to define zero and one wires)
    circuit.start();
    // > !SECTION - Setup

    // -------------------
    // > SECTION - Test 1: Assign values to a wire.
    // -------------------
    // This test executes the assign_value() on multiple wires to test the assigning of 0 and 1 values, and overflow values, both
    // positive and negative. In the overflow cases, the least significant wires should be the one assigned to the wire.
   
    // Wires to assign a value to
    Wire wire1;
    Wire wire2;
    Wire wire3;
    Wire wire4;

    // Assigning values
    circuit.assign_value(wire1, 0);
    circuit.assign_value(wire2, 1);
    circuit.assign_value(wire3, -56); // Overflow - Least significant wire is 0
    circuit.assign_value(wire4, 127); // Overflow - Least significant wire is 1

    // Checks
    REQUIRE(wire1.label == circuit._zero_wire.label);
    REQUIRE(wire2.label == circuit._one_wire.label);
    REQUIRE(wire3.label == circuit._zero_wire.label);
    REQUIRE(wire4.label == circuit._one_wire.label);
    // > !SECTION - Test 1: Assign values to a wire.

    // -------------------
    // > SECTION - Test 2: Assign values to a variable.
    // -------------------
    // This test executes the assign_value() on an unsigned value. This method should assign the labels from the zero_wire and
    // one_wire into the input variable wires, satisfying the specified input value to assign bit values.
    //
    // For example, assign_value 5 into a variable of 4 wires:
    // 1. Value 5: 0101
    // 2. Variable:
    //      2.1 Wires[3] = _zero_wire.
    //      2.2 Wires[2] = _one_wire;
    //      2.3 Wires[1] = _zero_wire;
    //      2.4 Wires[0] = _one_wire;
    //
    // This test will assign different values into 6 variables (with 8 wires):
    // 1. Variable will have value 0 (00000000);
    // 2. Variable will have value 255 (11111111);
    // 3. Variable will have value 123 (01111011);
    // 4. Variable will have value -57 (11000111);
    // 5. Variable will have value 36782 (1000111110101110) - overflow case;
    // 6. Variable will have value -4578 (10111000011110) - overflow case.
    // We can see that, in the last two cases, both need more than 8 wires. It is expected to have only the 8 least significant
    // bits assigned to the variable.

    // Variables to assign a value to
    Var var1(8);
    Var var2(8);
    Var var3(8);
    Var var4(8);
    Var var5(8);
    Var var6(8);

    // Assigning values
    circuit.assign_value(var1, 0);
    circuit.assign_value(var2, 255);
    circuit.assign_value(var3, 123);
    circuit.assign_value(var4, -57);
    circuit.assign_value(var5, 36782);
    circuit.assign_value(var6, -4578);

    // Checks - 1st variable
    REQUIRE(var1[7].label == circuit._zero_wire.label);
    REQUIRE(var1[6].label == circuit._zero_wire.label);
    REQUIRE(var1[5].label == circuit._zero_wire.label);
    REQUIRE(var1[4].label == circuit._zero_wire.label);
    REQUIRE(var1[3].label == circuit._zero_wire.label);
    REQUIRE(var1[2].label == circuit._zero_wire.label);
    REQUIRE(var1[1].label == circuit._zero_wire.label);
    REQUIRE(var1[0].label == circuit._zero_wire.label);

    // Checks - 2nd variable
    REQUIRE(var2[7].label == circuit._one_wire.label);
    REQUIRE(var2[6].label == circuit._one_wire.label);
    REQUIRE(var2[5].label == circuit._one_wire.label);
    REQUIRE(var2[4].label == circuit._one_wire.label);
    REQUIRE(var2[3].label == circuit._one_wire.label);
    REQUIRE(var2[2].label == circuit._one_wire.label);
    REQUIRE(var2[1].label == circuit._one_wire.label);
    REQUIRE(var2[0].label == circuit._one_wire.label);

    // Checks - 3rd variable
    REQUIRE(var3[7].label == circuit._zero_wire.label);
    REQUIRE(var3[6].label == circuit._one_wire.label);
    REQUIRE(var3[5].label == circuit._one_wire.label);
    REQUIRE(var3[4].label == circuit._one_wire.label);
    REQUIRE(var3[3].label == circuit._one_wire.label);
    REQUIRE(var3[2].label == circuit._zero_wire.label);
    REQUIRE(var3[1].label == circuit._one_wire.label);
    REQUIRE(var3[0].label == circuit._one_wire.label);

    // Checks - 4th variable
    REQUIRE(var4[7].label == circuit._one_wire.label);
    REQUIRE(var4[6].label == circuit._one_wire.label);
    REQUIRE(var4[5].label == circuit._zero_wire.label);
    REQUIRE(var4[4].label == circuit._zero_wire.label);
    REQUIRE(var4[3].label == circuit._zero_wire.label);
    REQUIRE(var4[2].label == circuit._one_wire.label);
    REQUIRE(var4[1].label == circuit._one_wire.label);
    REQUIRE(var4[0].label == circuit._one_wire.label);

    // Checks - 5th variable
    REQUIRE(var5[7].label == circuit._one_wire.label);
    REQUIRE(var5[6].label == circuit._zero_wire.label);
    REQUIRE(var5[5].label == circuit._one_wire.label);
    REQUIRE(var5[4].label == circuit._zero_wire.label);
    REQUIRE(var5[3].label == circuit._one_wire.label);
    REQUIRE(var5[2].label == circuit._one_wire.label);
    REQUIRE(var5[1].label == circuit._one_wire.label);
    REQUIRE(var5[0].label == circuit._zero_wire.label);   

    // Checks - 6th variable
    REQUIRE(var6[7].label == circuit._zero_wire.label);
    REQUIRE(var6[6].label == circuit._zero_wire.label);
    REQUIRE(var6[5].label == circuit._zero_wire.label);
    REQUIRE(var6[4].label == circuit._one_wire.label);
    REQUIRE(var6[3].label == circuit._one_wire.label);
    REQUIRE(var6[2].label == circuit._one_wire.label);
    REQUIRE(var6[1].label == circuit._one_wire.label);
    REQUIRE(var6[0].label == circuit._zero_wire.label);   
    // > !SECTION - Test 2: Assign values to a variable.
    // > !SECTION - Test suit "Assign Value Operations"
}

TEST_CASE("Shift Operations") {
    // > SECTION - Test suit "Shift Operations"
    //
    // This test suit aims to test the shift operations of the Circuit Generator class.
    //
    // Note: The constructor of CircuitGenerator does not have a public visibility, thus an instance cannot be created.
    // For the sake of the tests, the tester class makes it accessible.
    //
    // Test suit index:
    // > Test 1: Shift left variable overriding it
    // > Test 2: Shift left variable into new variable
    // > Test 3: Shift right variable overriding it
    // > Test 4: Shift right variable into new variable

    // -------------------
    // > SECTION - Setup
    // -------------------
    // This section creates a circuit generator and prepares a circuit with 2 input parties, both with 8 dedicated input wires,
    // and a single output party, also of 8 wires.
    // The methods used in this setup already have tests covering them in Test suit "Core".

    // Circuit Generator constructor
    CircuitGeneratorTester circuit("ShiftOperations", "circuits/tests");

    // Parties
    circuit.add_input_party(8);
    circuit.add_input_party(8);
    circuit.add_output_party(8);

    // Inputs
    Var in_1(8), in_2(8);
    circuit.add_input(in_1);
    circuit.add_input(in_2);

    // Outputs
    Var out(8);
    circuit.add_output(out);
    
    // Starts the circuit (to define zero and one wires)
    circuit.start();
    // > !SECTION - Setup

    // -------------------
    // > SECTION - Test 1: Shift left variable overriding it.
    // -------------------
    // This test executes the shift_left() on the input variable, overriding it. This method expects an input variable as well as
    // the amount of bits to shift.
    // 
    // Note: Since this amount of bits to shift is an unsigned value, it does not expects a negative value to be given (as if
    // trying to shift right instead). If provided as negative, this value will be converted to unsigned and, in the end, the
    // function does not behave as expected.
    //
    // It is expected that:
    // 1. The input variable is updated with the shifted result;
    // 2. The wires should be shifted towards the most significant wires;
    // 3. The new wires that appear in the right should have their labels with the _zero_wire label.
    // 
    // This test includes the following sub-tests: (all the variables have 8 bits)
    // 1. Shift value 123 (01111011) variable 0 bits to the left. This should result in the same variable value;
    // 2. Shift value 123 (01111011) variable 3 bits to the left. This should result in a variable with value 216/-40 (11011000);
    // 3. Shift value 216/-40 (11011000) variable 10 bits to the left. This should result in a variable with value 0 (00000000);

    // Variable to test
    Var var_t1(8);
    circuit.assign_value(var_t1, 123);

    // Check shift 0 bits on value 123
    circuit.shift_left(var_t1, 0);
    REQUIRE(var_t1[7].label == circuit._zero_wire.label);
    REQUIRE(var_t1[6].label == circuit._one_wire.label);
    REQUIRE(var_t1[5].label == circuit._one_wire.label);
    REQUIRE(var_t1[4].label == circuit._one_wire.label);
    REQUIRE(var_t1[3].label == circuit._one_wire.label);
    REQUIRE(var_t1[2].label == circuit._zero_wire.label);
    REQUIRE(var_t1[1].label == circuit._one_wire.label);
    REQUIRE(var_t1[0].label == circuit._one_wire.label);

    // Check shift 3 bits on value 123
    circuit.shift_left(var_t1, 3);
    REQUIRE(var_t1[7].label == circuit._one_wire.label);
    REQUIRE(var_t1[6].label == circuit._one_wire.label);
    REQUIRE(var_t1[5].label == circuit._zero_wire.label);
    REQUIRE(var_t1[4].label == circuit._one_wire.label);
    REQUIRE(var_t1[3].label == circuit._one_wire.label);
    REQUIRE(var_t1[2].label == circuit._zero_wire.label);
    REQUIRE(var_t1[1].label == circuit._zero_wire.label);
    REQUIRE(var_t1[0].label == circuit._zero_wire.label);

    // Check shift 10 bits on value 216/-40
    circuit.shift_left(var_t1, 10);
    REQUIRE(var_t1[7].label == circuit._zero_wire.label);
    REQUIRE(var_t1[6].label == circuit._zero_wire.label);
    REQUIRE(var_t1[5].label == circuit._zero_wire.label);
    REQUIRE(var_t1[4].label == circuit._zero_wire.label);
    REQUIRE(var_t1[3].label == circuit._zero_wire.label);
    REQUIRE(var_t1[2].label == circuit._zero_wire.label);
    REQUIRE(var_t1[1].label == circuit._zero_wire.label);
    REQUIRE(var_t1[0].label == circuit._zero_wire.label);
    // > !SECTION - Test 1: Shift left variable overriding it.

    // -------------------
    // > SECTION - Test 2: Shift left variable into new variable.
    // -------------------
    // This test executes the shift_left() on the input variable, outputing the result into another variable. This method expects
    // an input variable, the amount of bits to shift, and another input variable on which the result will be written. 
    //
    // Note: Since this amount of bits to shift is an unsigned value, it does not expects a negative value to be given (as if
    // trying to shift right instead). If provided as negative, this value will be converted to unsigned and, in the end, the
    // function does not behave as expected.
    //
    // It is expected that:
    // 1. The input variable is updated with the shifted result;
    // 2. The wires should be shifted towards the most significant wires;
    // 3. The new wires that appear in the right should have their labels with the _zero_wire label.
    // 
    // This test includes the following sub-tests: (all the variables have 8 bits)
    // 1. Shift value 123 (01111011) variable 0 bits to the left. This should result in the same variable value;
    // 2. Shift value 123 (01111011) variable 3 bits to the left. This should result in a variable with value 216/-40 (11011000);
    // 3. Shift value 123 (01111011) variable 10 bits to the left. This should result in a variable with value 0 (00000000);
    // 4. Shift value 123 (01111011) variable 3 bits to the left, giving the same input and output variable. This should result in
    // the override of the input variable with value 216/-40 (11011000);

    // Variable to test
    Var var_t2(8);
    Var var_t2_out(8);
    circuit.assign_value(var_t2, 123);

    // Shift 0 bits on value 123
    circuit.shift_left(var_t2, 0, var_t2_out);

    // Check input variable
    REQUIRE(var_t2[7].label == circuit._zero_wire.label);
    REQUIRE(var_t2[6].label == circuit._one_wire.label);
    REQUIRE(var_t2[5].label == circuit._one_wire.label);
    REQUIRE(var_t2[4].label == circuit._one_wire.label);
    REQUIRE(var_t2[3].label == circuit._one_wire.label);
    REQUIRE(var_t2[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2[1].label == circuit._one_wire.label);
    REQUIRE(var_t2[0].label == circuit._one_wire.label);

    // Check output variable
    REQUIRE(var_t2_out[7].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[6].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[5].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[4].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[3].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[1].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[0].label == circuit._one_wire.label);

    // Shift 3 bits on value 123
    circuit.shift_left(var_t2, 3, var_t2_out);

    // Check input variable
    REQUIRE(var_t2[7].label == circuit._zero_wire.label);
    REQUIRE(var_t2[6].label == circuit._one_wire.label);
    REQUIRE(var_t2[5].label == circuit._one_wire.label);
    REQUIRE(var_t2[4].label == circuit._one_wire.label);
    REQUIRE(var_t2[3].label == circuit._one_wire.label);
    REQUIRE(var_t2[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2[1].label == circuit._one_wire.label);
    REQUIRE(var_t2[0].label == circuit._one_wire.label);

    // Check output variable
    REQUIRE(var_t2_out[7].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[6].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[5].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[4].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[3].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[1].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[0].label == circuit._zero_wire.label);

    // Shift 10 bits on value 123
    circuit.shift_left(var_t2, 10, var_t2_out);

    // Check input variable
    REQUIRE(var_t2[7].label == circuit._zero_wire.label);
    REQUIRE(var_t2[6].label == circuit._one_wire.label);
    REQUIRE(var_t2[5].label == circuit._one_wire.label);
    REQUIRE(var_t2[4].label == circuit._one_wire.label);
    REQUIRE(var_t2[3].label == circuit._one_wire.label);
    REQUIRE(var_t2[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2[1].label == circuit._one_wire.label);
    REQUIRE(var_t2[0].label == circuit._one_wire.label);

    // Check output variable
    REQUIRE(var_t2_out[7].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[6].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[5].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[4].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[3].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[1].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[0].label == circuit._zero_wire.label);

    // Shift 3 bits on value 123, giving the same variable as input and output
    circuit.shift_left(var_t2, 3, var_t2);

    // Check output variable
    REQUIRE(var_t2[7].label == circuit._one_wire.label);
    REQUIRE(var_t2[6].label == circuit._one_wire.label);
    REQUIRE(var_t2[5].label == circuit._zero_wire.label);
    REQUIRE(var_t2[4].label == circuit._one_wire.label);
    REQUIRE(var_t2[3].label == circuit._one_wire.label);
    REQUIRE(var_t2[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2[1].label == circuit._zero_wire.label);
    REQUIRE(var_t2[0].label == circuit._zero_wire.label);
    // > !SECTION - Test 2: Shift left variable into new variable.

    // -------------------
    // > SECTION - Test 3: Shift right variable overriding it.
    // -------------------
    // This test executes the shift_right() on the input variable, overriding it. This method expects an input variable as well as
    // the amount of bits to shift.
    // 
    // Note: Since this amount of bits to shift is an unsigned value, it does not expects a negative value to be given (as if
    // trying to shift right instead). If provided as negative, this value will be converted to unsigned and, in the end, the
    // function does not behave as expected.
    //
    // It is expected that:
    // 1. The input variable is updated with the shifted result;
    // 2. The wires should be shifted towards the least significant wires;
    // 3. The new wires that appear in the left should have their labels with the _zero_wire label.
    // 
    // This test includes the following sub-tests: (all the variables have 8 bits)
    // 1. Shift value 251/-5 (11111011) variable 0 bits to the right. This should result in the same variable value;
    // 2. Shift value 251/-5 (11111011) variable 3 bits to the right. This should result in a variable with value 31 (00011111);
    // 3. Shift value 31 (00011111) variable 10 bits to the right. This should result in a variable with value 0 (00000000);

    // Variable to test
    Var var_t3(8);
    circuit.assign_value(var_t3, 251);

    // Check shift 0 bits on value 251/-5
    circuit.shift_right(var_t3, 0);
    REQUIRE(var_t3[7].label == circuit._one_wire.label);
    REQUIRE(var_t3[6].label == circuit._one_wire.label);
    REQUIRE(var_t3[5].label == circuit._one_wire.label);
    REQUIRE(var_t3[4].label == circuit._one_wire.label);
    REQUIRE(var_t3[3].label == circuit._one_wire.label);
    REQUIRE(var_t3[2].label == circuit._zero_wire.label);
    REQUIRE(var_t3[1].label == circuit._one_wire.label);
    REQUIRE(var_t3[0].label == circuit._one_wire.label);

    // Check shift 3 bits on value 251/-5
    circuit.shift_right(var_t3, 3);
    REQUIRE(var_t3[7].label == circuit._zero_wire.label);
    REQUIRE(var_t3[6].label == circuit._zero_wire.label);
    REQUIRE(var_t3[5].label == circuit._zero_wire.label);
    REQUIRE(var_t3[4].label == circuit._one_wire.label);
    REQUIRE(var_t3[3].label == circuit._one_wire.label);
    REQUIRE(var_t3[2].label == circuit._one_wire.label);
    REQUIRE(var_t3[1].label == circuit._one_wire.label);
    REQUIRE(var_t3[0].label == circuit._one_wire.label);

    // Check shift 10 bits on value 31
    circuit.shift_right(var_t3, 10);
    REQUIRE(var_t3[7].label == circuit._zero_wire.label);
    REQUIRE(var_t3[6].label == circuit._zero_wire.label);
    REQUIRE(var_t3[5].label == circuit._zero_wire.label);
    REQUIRE(var_t3[4].label == circuit._zero_wire.label);
    REQUIRE(var_t3[3].label == circuit._zero_wire.label);
    REQUIRE(var_t3[2].label == circuit._zero_wire.label);
    REQUIRE(var_t3[1].label == circuit._zero_wire.label);
    REQUIRE(var_t3[0].label == circuit._zero_wire.label);
    // > !SECTION - Test 3: Shift right variable overriding it.

    // -------------------
    // > SECTION - Test 4: Shift right variable into new variable.
    // -------------------
    // This test executes the shift_right() on the input variable, outputing the result into another variable. This method expects
    // an input variable, the amount of bits to shift, and another input variable on which the result will be written.
    // 
    // Note: Since this amount of bits to shift is an unsigned value, it does not expects a negative value to be given (as if
    // trying to shift right instead). If provided as negative, this value will be converted to unsigned and, in the end, the
    // function does not behave as expected.
    // 
    // It is expected that:
    // 1. The input variable is updated with the shifted result;
    // 2. The wires should be shifted towards the least significant wires;
    // 3. The new wires that appear in the left should have their labels with the _zero_wire label.
    // 
    // This test includes the following sub-tests: (all the variables have 8 bits)
    // 1. Shift value 251/-5 (11111011) variable 0 bits to the right. This should result in the same variable value;
    // 2. Shift value 251/-5 (11111011) variable 3 bits to the right. This should result in a variable with value 31 (00011111);
    // 3. Shift value 251/-5 (11111011) variable 10 bits to the right. This should result in a variable with value 0 (00000000);
    // 4. Shift value 251/-5 (11111011) variable 3 bits to the left, giving the same input and output variable. This should result
    // in the override of the input variable with value 31 (00011111);

    // Variable to test
    Var var_t4(8);
    Var var_t4_out(8);
    circuit.assign_value(var_t4, 251);

    // Shift 0 bits on value 251/-5
    circuit.shift_right(var_t4, 0, var_t4_out);

    // Check input variable
    REQUIRE(var_t4[7].label == circuit._one_wire.label);
    REQUIRE(var_t4[6].label == circuit._one_wire.label);
    REQUIRE(var_t4[5].label == circuit._one_wire.label);
    REQUIRE(var_t4[4].label == circuit._one_wire.label);
    REQUIRE(var_t4[3].label == circuit._one_wire.label);
    REQUIRE(var_t4[2].label == circuit._zero_wire.label);
    REQUIRE(var_t4[1].label == circuit._one_wire.label);
    REQUIRE(var_t4[0].label == circuit._one_wire.label);

    // Check output variable
    REQUIRE(var_t4_out[7].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[6].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[5].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[4].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[3].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[2].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[1].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[0].label == circuit._one_wire.label);

    // Shift 3 bits on value 251/-5
    circuit.shift_right(var_t4, 3, var_t4_out);

    // Check input variable
    REQUIRE(var_t4[7].label == circuit._one_wire.label);
    REQUIRE(var_t4[6].label == circuit._one_wire.label);
    REQUIRE(var_t4[5].label == circuit._one_wire.label);
    REQUIRE(var_t4[4].label == circuit._one_wire.label);
    REQUIRE(var_t4[3].label == circuit._one_wire.label);
    REQUIRE(var_t4[2].label == circuit._zero_wire.label);
    REQUIRE(var_t4[1].label == circuit._one_wire.label);
    REQUIRE(var_t4[0].label == circuit._one_wire.label);

    // Check output variable
    REQUIRE(var_t4_out[7].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[6].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[5].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[4].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[3].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[2].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[1].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[0].label == circuit._one_wire.label);

    // Shift 10 bits on value 251/-5
    circuit.shift_left(var_t4, 10, var_t4_out);

    // Check input variable
    REQUIRE(var_t4[7].label == circuit._one_wire.label);
    REQUIRE(var_t4[6].label == circuit._one_wire.label);
    REQUIRE(var_t4[5].label == circuit._one_wire.label);
    REQUIRE(var_t4[4].label == circuit._one_wire.label);
    REQUIRE(var_t4[3].label == circuit._one_wire.label);
    REQUIRE(var_t4[2].label == circuit._zero_wire.label);
    REQUIRE(var_t4[1].label == circuit._one_wire.label);
    REQUIRE(var_t4[0].label == circuit._one_wire.label);

    // Check output variable
    REQUIRE(var_t4_out[7].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[6].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[5].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[4].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[3].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[2].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[1].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[0].label == circuit._zero_wire.label);

    // Shift 3 bits on value 251/-5, giving the same variable as input and output
    circuit.shift_right(var_t4, 3, var_t4);

    // Check output variable
    REQUIRE(var_t4[7].label == circuit._zero_wire.label);
    REQUIRE(var_t4[6].label == circuit._zero_wire.label);
    REQUIRE(var_t4[5].label == circuit._zero_wire.label);
    REQUIRE(var_t4[4].label == circuit._one_wire.label);
    REQUIRE(var_t4[3].label == circuit._one_wire.label);
    REQUIRE(var_t4[2].label == circuit._one_wire.label);
    REQUIRE(var_t4[1].label == circuit._one_wire.label);
    REQUIRE(var_t4[0].label == circuit._one_wire.label);
    // > !SECTION - Test 4: Shift right variable into new variable.
    // > !SECTION - Test suit "Shift Operations"
}

TEST_CASE("Rotate Operations") {
    // > SECTION - Test suit "Rotate Operations"
    // 
    // This test suit aims to test the rotate operations of the Circuit Generator class.
    // 
    // Note: The constructor of CircuitGenerator does not have a public visibility, thus an instance cannot be created.
    // For the sake of the tests, the tester class makes it accessible.
    //
    // Test suit index:
    // > Test 1: Rotate left variable overriding it
    // > Test 2: Rotate left variable into new variable
    // > Test 3: Rotate right variable overriding it
    // > Test 4: Rotate right variable into new variable

    // -------------------
    // > SECTION - Setup
    // -------------------
    // This section creates a circuit generator and prepares a circuit with 2 input parties, both with 8 dedicated input wires,
    // and a single output party, also of 8 wires.
    // The methods used in this setup already have tests covering them in Test suit "Core".

    // Circuit Generator constructor
    CircuitGeneratorTester circuit("RotateOperations", "circuits/tests");

    // Parties
    circuit.add_input_party(8);
    circuit.add_input_party(8);
    circuit.add_output_party(8);

    // Inputs
    Var in_1(8), in_2(8);
    circuit.add_input(in_1);
    circuit.add_input(in_2);

    // Outputs
    Var out(8);
    circuit.add_output(out);
    
    // Starts the circuit (to define zero and one wires)
    circuit.start();
    // > !SECTION - Setup

    // -------------------
    // > SECTION - Test 1: Rotate left variable overriding it.
    // -------------------
    // This test executes the rotate_left() on the input variable, overriding it. This method expects an input variable as well as
    // the amount of bits to rotate.
    // 
    // Note: Since this amount of bits to rotate is an unsigned value, it does not expects a negative value to be given (as if
    // trying to rotate right instead). If provided as negative, this value will be converted to unsigned and, in the end, the
    // function does not behave as expected.
    // 
    // It is expected that:
    // 1. The input variable is updated with the rotated result;
    // 2. The wires should be shift towards the most significant wires;
    // 3. The most significant wires that shifted "out of bounds" return to the other side of the variable, i.e as least
    // significant wires.
    // 
    // This test includes the following sub-tests: (all the variables have 8 bits)
    // 1. Rotate value 123 (01111011) variable 0 bits to the left. This should result in the same variable value;
    // 2. Rotate value 123 (01111011) variable 3 bits to the left. This should result in a variable with value 219/-37 (11011011);
    // 3. Rotate value 219/-37 (11011011) variable 13 bits to the left. This should result in a variable with value 123 (01111011);
    
    // Variable to test
    Var var_t1(8);
    circuit.assign_value(var_t1, 123);

    // Check rotate 0 bits on value 123
    circuit.rotate_left(var_t1, 0);
    REQUIRE(var_t1[7].label == circuit._zero_wire.label);
    REQUIRE(var_t1[6].label == circuit._one_wire.label);
    REQUIRE(var_t1[5].label == circuit._one_wire.label);
    REQUIRE(var_t1[4].label == circuit._one_wire.label);
    REQUIRE(var_t1[3].label == circuit._one_wire.label);
    REQUIRE(var_t1[2].label == circuit._zero_wire.label);
    REQUIRE(var_t1[1].label == circuit._one_wire.label);
    REQUIRE(var_t1[0].label == circuit._one_wire.label);

    // Check rotate 3 bits on value 123
    circuit.rotate_left(var_t1, 3);
    REQUIRE(var_t1[7].label == circuit._one_wire.label);
    REQUIRE(var_t1[6].label == circuit._one_wire.label);
    REQUIRE(var_t1[5].label == circuit._zero_wire.label);
    REQUIRE(var_t1[4].label == circuit._one_wire.label);
    REQUIRE(var_t1[3].label == circuit._one_wire.label);
    REQUIRE(var_t1[2].label == circuit._zero_wire.label);
    REQUIRE(var_t1[1].label == circuit._one_wire.label);
    REQUIRE(var_t1[0].label == circuit._one_wire.label);

    // Check rotate 13 bits on value 219/-37
    circuit.rotate_left(var_t1, 13);
    REQUIRE(var_t1[7].label == circuit._zero_wire.label);
    REQUIRE(var_t1[6].label == circuit._one_wire.label);
    REQUIRE(var_t1[5].label == circuit._one_wire.label);
    REQUIRE(var_t1[4].label == circuit._one_wire.label);
    REQUIRE(var_t1[3].label == circuit._one_wire.label);
    REQUIRE(var_t1[2].label == circuit._zero_wire.label);
    REQUIRE(var_t1[1].label == circuit._one_wire.label);
    REQUIRE(var_t1[0].label == circuit._one_wire.label);
    // > !SECTION - Test 1: Rotate left variable overriding it.

    // -------------------
    // > SECTION - Test 2: Rotate left variable into new variable.
    // -------------------
    // This test executes the rotate_left() on the input variable, outputing the result into another variable. This method expects
    // an input variable, the amount of bits to rotate, and another input variable on which the result will be written. 
    //
    // Note: Since this amount of bits to rotate is an unsigned value, it does not expects a negative value to be given (as if
    // trying to rotate right instead). If provided as negative, this value will be converted to unsigned and, in the end, the
    // function does not behave as expected.
    //
    // It is expected that:
    // 1. The input variable is updated with the rotated result;
    // 2. The wires should be shift towards the most significant wires;
    // 3. The most significant wires that shifted "out of bounds" return to the other side of the variable, i.e as least
    // significant wires.
    // 
    // This test includes the following sub-tests: (all the variables have 8 bits)
    // 1. Rotate value 123 (01111011) variable 0 bits to the left. This should result in the same variable value;
    // 2. Rotate value 123 (01111011) variable 3 bits to the left. This should result in a variable with value 219/-37 (11011011);
    // 3. Rotate value 123 (01111011) variable 8 bits to the left. This should result in a variable with value 123 (01111011);
    // 4. Rotate value 123 (01111011) variable 3 bits to the left, giving the same input and output variable. This should result in
    // the override of the input variable with value 219/-37 (11011011);

    // Variable to test
    Var var_t2(8);
    Var var_t2_out(8);
    circuit.assign_value(var_t2, 123);

    // Rotate 0 bits on value 123
    circuit.rotate_left(var_t2, 0, var_t2_out);

    // Check input variable
    REQUIRE(var_t2[7].label == circuit._zero_wire.label);
    REQUIRE(var_t2[6].label == circuit._one_wire.label);
    REQUIRE(var_t2[5].label == circuit._one_wire.label);
    REQUIRE(var_t2[4].label == circuit._one_wire.label);
    REQUIRE(var_t2[3].label == circuit._one_wire.label);
    REQUIRE(var_t2[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2[1].label == circuit._one_wire.label);
    REQUIRE(var_t2[0].label == circuit._one_wire.label);

    // Check output variable
    REQUIRE(var_t2_out[7].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[6].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[5].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[4].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[3].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[1].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[0].label == circuit._one_wire.label);

    // Rotate 3 bits on value 123
    circuit.rotate_left(var_t2, 3, var_t2_out);

    // Check input variable
    REQUIRE(var_t2[7].label == circuit._zero_wire.label);
    REQUIRE(var_t2[6].label == circuit._one_wire.label);
    REQUIRE(var_t2[5].label == circuit._one_wire.label);
    REQUIRE(var_t2[4].label == circuit._one_wire.label);
    REQUIRE(var_t2[3].label == circuit._one_wire.label);
    REQUIRE(var_t2[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2[1].label == circuit._one_wire.label);
    REQUIRE(var_t2[0].label == circuit._one_wire.label);

    // Check output variable
    REQUIRE(var_t2_out[7].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[6].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[5].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[4].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[3].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[1].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[0].label == circuit._one_wire.label);

    // Rotate 8 bits on value 123
    circuit.rotate_left(var_t2, 8, var_t2_out);

    // Check input variable
    REQUIRE(var_t2[7].label == circuit._zero_wire.label);
    REQUIRE(var_t2[6].label == circuit._one_wire.label);
    REQUIRE(var_t2[5].label == circuit._one_wire.label);
    REQUIRE(var_t2[4].label == circuit._one_wire.label);
    REQUIRE(var_t2[3].label == circuit._one_wire.label);
    REQUIRE(var_t2[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2[1].label == circuit._one_wire.label);
    REQUIRE(var_t2[0].label == circuit._one_wire.label);

    // Check output variable
    REQUIRE(var_t2_out[7].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[6].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[5].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[4].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[3].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[1].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[0].label == circuit._one_wire.label);

    // Rotate 3 bits on value 123, giving the same variable as input and output
    circuit.rotate_left(var_t2, 3, var_t2);

    // Check output variable
    REQUIRE(var_t2[7].label == circuit._one_wire.label);
    REQUIRE(var_t2[6].label == circuit._one_wire.label);
    REQUIRE(var_t2[5].label == circuit._zero_wire.label);
    REQUIRE(var_t2[4].label == circuit._one_wire.label);
    REQUIRE(var_t2[3].label == circuit._one_wire.label);
    REQUIRE(var_t2[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2[1].label == circuit._one_wire.label);
    REQUIRE(var_t2[0].label == circuit._one_wire.label);
    // > !SECTION - Test 2: Rotate left variable into new variable.

    // -------------------
    // > SECTION - Test 3: Rotate right variable overriding it.
    // -------------------
    // This test executes the rotate_right() on the input variable, overriding it. This method expects an input variable as well
    // as the amount of bits to rotate.
    // 
    // Note: Since this amount of bits to rotate is an unsigned value, it does not expects a negative value to be given (as if
    // trying to rotate left instead). If provided as negative, this value will be converted to unsigned and, in the end, the
    // function does not behave as expected.
    // 
    // It is expected that:
    // 1. The input variable is updated with the rotated result;
    // 2. The wires should be shift towards the least significant wires;
    // 3. The least significant wires that shifted "out of bounds" return to the other side of the variable, i.e as most
    // significant wires.
    // 
    // This test includes the following sub-tests: (all the variables have 8 bits)
    // 1. Rotate value 123 (01111011) variable 0 bits to the right. This should result in the same variable value;
    // 2. Rotate value 123 (01111011) variable 3 bits to the right. This should result in a variable with value 111 (01101111);
    // 3. Rotate value 111 (01101111) variable 13 bits to the right. This should result in a variable with value 123 (01111011);
    
    // Variable to test
    Var var_t3(8);
    circuit.assign_value(var_t3, 123);

    // Check rotate 0 bits on value 123
    circuit.rotate_right(var_t3, 0);
    REQUIRE(var_t3[7].label == circuit._zero_wire.label);
    REQUIRE(var_t3[6].label == circuit._one_wire.label);
    REQUIRE(var_t3[5].label == circuit._one_wire.label);
    REQUIRE(var_t3[4].label == circuit._one_wire.label);
    REQUIRE(var_t3[3].label == circuit._one_wire.label);
    REQUIRE(var_t3[2].label == circuit._zero_wire.label);
    REQUIRE(var_t3[1].label == circuit._one_wire.label);
    REQUIRE(var_t3[0].label == circuit._one_wire.label);

    // Check rotate 3 bits on value 123
    circuit.rotate_right(var_t3, 3);
    REQUIRE(var_t3[7].label == circuit._zero_wire.label);
    REQUIRE(var_t3[6].label == circuit._one_wire.label);
    REQUIRE(var_t3[5].label == circuit._one_wire.label);
    REQUIRE(var_t3[4].label == circuit._zero_wire.label);
    REQUIRE(var_t3[3].label == circuit._one_wire.label);
    REQUIRE(var_t3[2].label == circuit._one_wire.label);
    REQUIRE(var_t3[1].label == circuit._one_wire.label);
    REQUIRE(var_t3[0].label == circuit._one_wire.label);

    // Check rotate 13 bits on value 111
    circuit.rotate_right(var_t3, 13);
    REQUIRE(var_t3[7].label == circuit._zero_wire.label);
    REQUIRE(var_t3[6].label == circuit._one_wire.label);
    REQUIRE(var_t3[5].label == circuit._one_wire.label);
    REQUIRE(var_t3[4].label == circuit._one_wire.label);
    REQUIRE(var_t3[3].label == circuit._one_wire.label);
    REQUIRE(var_t3[2].label == circuit._zero_wire.label);
    REQUIRE(var_t3[1].label == circuit._one_wire.label);
    REQUIRE(var_t3[0].label == circuit._one_wire.label);
    // > !SECTION - Test 3: Rotate right variable overriding it.

    // -------------------
    // > SECTION - Test 4: Rotate right variable into new variable.
    // -------------------
    // This test executes the rotate_right() on the input variable, outputing the result into another variable. This method
    // expects an input variable, the amount of bits to rotate, and another input variable on which the result will be written. 
    //
    // Note: Since this amount of bits to rotate is an unsigned value, it does not expects a negative value to be given (as if
    // trying to rotate left instead). If provided as negative, this value will be converted to unsigned and, in the end, the
    // function does not behave as expected.
    //
    // It is expected that:
    // 1. The input variable is updated with the rotated result;
    // 2. The wires should be shift towards the least significant wires;
    // 3. The least significant wires that shifted "out of bounds" return to the other side of the variable, i.e as most
    // significant wires.
    // 
    // This test includes the following sub-tests: (all the variables have 8 bits)
    // 1. Rotate value 123 (01111011) variable 0 bits to the right. This should result in the same variable value;
    // 2. Rotate value 123 (01111011) variable 3 bits to the right. This should result in a variable with value 111 (01101111);
    // 3. Rotate value 123 (01111011) variable 8 bits to the right. This should result in a variable with value 123 (01111011);
    // 4. Rotate value 123 (01111011) variable 3 bits to the right, giving the same input and output variable. This should result
    // in the override of the input variable with value 111 (01101111);

    // Variable to test
    Var var_t4(8);
    Var var_t4_out(8);
    circuit.assign_value(var_t4, 123);

    // Rotate 0 bits on value 123
    circuit.rotate_right(var_t4, 0, var_t4_out);

    // Check input variable
    REQUIRE(var_t4[7].label == circuit._zero_wire.label);
    REQUIRE(var_t4[6].label == circuit._one_wire.label);
    REQUIRE(var_t4[5].label == circuit._one_wire.label);
    REQUIRE(var_t4[4].label == circuit._one_wire.label);
    REQUIRE(var_t4[3].label == circuit._one_wire.label);
    REQUIRE(var_t4[2].label == circuit._zero_wire.label);
    REQUIRE(var_t4[1].label == circuit._one_wire.label);
    REQUIRE(var_t4[0].label == circuit._one_wire.label);

    // Check output variable
    REQUIRE(var_t4_out[7].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[6].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[5].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[4].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[3].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[2].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[1].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[0].label == circuit._one_wire.label);

    // Rotate 3 bits on value 123
    circuit.rotate_right(var_t4, 3, var_t4_out);

    // Check input variable
    REQUIRE(var_t4[7].label == circuit._zero_wire.label);
    REQUIRE(var_t4[6].label == circuit._one_wire.label);
    REQUIRE(var_t4[5].label == circuit._one_wire.label);
    REQUIRE(var_t4[4].label == circuit._one_wire.label);
    REQUIRE(var_t4[3].label == circuit._one_wire.label);
    REQUIRE(var_t4[2].label == circuit._zero_wire.label);
    REQUIRE(var_t4[1].label == circuit._one_wire.label);
    REQUIRE(var_t4[0].label == circuit._one_wire.label);

    // Check output variable
    REQUIRE(var_t4_out[7].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[6].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[5].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[4].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[3].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[2].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[1].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[0].label == circuit._one_wire.label);

    // Rotate 8 bits on value 123
    circuit.rotate_right(var_t4, 8, var_t4_out);

    // Check input variable
    REQUIRE(var_t4[7].label == circuit._zero_wire.label);
    REQUIRE(var_t4[6].label == circuit._one_wire.label);
    REQUIRE(var_t4[5].label == circuit._one_wire.label);
    REQUIRE(var_t4[4].label == circuit._one_wire.label);
    REQUIRE(var_t4[3].label == circuit._one_wire.label);
    REQUIRE(var_t4[2].label == circuit._zero_wire.label);
    REQUIRE(var_t4[1].label == circuit._one_wire.label);
    REQUIRE(var_t4[0].label == circuit._one_wire.label);

    // Check output variable
    REQUIRE(var_t4_out[7].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[6].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[5].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[4].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[3].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[2].label == circuit._zero_wire.label);
    REQUIRE(var_t4_out[1].label == circuit._one_wire.label);
    REQUIRE(var_t4_out[0].label == circuit._one_wire.label);

    // Rotate 3 bits on value 123, giving the same variable as input and output
    circuit.rotate_right(var_t4, 3, var_t4);

    // Check output variable
    REQUIRE(var_t4[7].label == circuit._zero_wire.label);
    REQUIRE(var_t4[6].label == circuit._one_wire.label);
    REQUIRE(var_t4[5].label == circuit._one_wire.label);
    REQUIRE(var_t4[4].label == circuit._zero_wire.label);
    REQUIRE(var_t4[3].label == circuit._one_wire.label);
    REQUIRE(var_t4[2].label == circuit._one_wire.label);
    REQUIRE(var_t4[1].label == circuit._one_wire.label);
    REQUIRE(var_t4[0].label == circuit._one_wire.label);
    // > !SECTION - Test 4: Rotate right variable into new variable.
    // > !SECTION - Test suit "Rotate Operations"
}

TEST_CASE("Flip Operations") {
    // > SECTION - Test suit "Flip Operations"
    //
    // This test suit aims to test the flip operations of the Circuit Generator class.
    //
    // Note: The constructor of CircuitGenerator does not have a public visibility, thus an instance cannot be created.
    // For the sake of the tests, the tester class makes it accessible.
    //
    // Test suit index:
    // > Test 1: Flip variable overriding it
    // > Test 2: Flip variable into new variable

    // -------------------
    // > SECTION - Setup
    // -------------------
    // This section creates a circuit generator and prepares a circuit with 2 input parties, both with 8 dedicated input wires,
    // and a single output party, also of 8 wires.
    // The methods used in this setup already have tests covering them in Test suit "Core".

    // Circuit Generator constructor
    CircuitGeneratorTester circuit("FlipOperations", "circuits/tests");

    // Parties
    circuit.add_input_party(8);
    circuit.add_input_party(8);
    circuit.add_output_party(8);

    // Inputs
    Var in_1(8), in_2(8);
    circuit.add_input(in_1);
    circuit.add_input(in_2);

    // Outputs
    Var out(8);
    circuit.add_output(out);
    
    // Starts the circuit (to define zero and one wires)
    circuit.start();
    // > !SECTION - Setup

    // -------------------
    // > SECTION - Test 1: Flip variable overriding it.
    // -------------------
    // This test executes the flip() on the input variable, overriding it. This method just expects an input variable to flip.
    // 
    // It is expected that:
    // 1. The input variable is updated with the flipped result;
    // 2. The wires should be mirrored, i.e the most significant wires are converted to least significant wires and vice-versa;
    // 
    // This test includes the following sub-tests: (all the variables have 8 bits)
    // 1. Flip value 15 (00001111). This should result in a variable with value 240/-16 (11110000);
    // 2. Flip value 85 (01010101). This should result in a variable with value 170/-86 (10101010);
    Var var_t1(8);

    // Check flip on value 15
    circuit.assign_value(var_t1, 15);
    circuit.flip(var_t1);
    REQUIRE(var_t1[7].label == circuit._one_wire.label);
    REQUIRE(var_t1[6].label == circuit._one_wire.label);
    REQUIRE(var_t1[5].label == circuit._one_wire.label);
    REQUIRE(var_t1[4].label == circuit._one_wire.label);
    REQUIRE(var_t1[3].label == circuit._zero_wire.label);
    REQUIRE(var_t1[2].label == circuit._zero_wire.label);
    REQUIRE(var_t1[1].label == circuit._zero_wire.label);
    REQUIRE(var_t1[0].label == circuit._zero_wire.label);

    // Check flip on value 85
    circuit.assign_value(var_t1, 85);
    circuit.flip(var_t1);
    REQUIRE(var_t1[7].label == circuit._one_wire.label);
    REQUIRE(var_t1[6].label == circuit._zero_wire.label);
    REQUIRE(var_t1[5].label == circuit._one_wire.label);
    REQUIRE(var_t1[4].label == circuit._zero_wire.label);
    REQUIRE(var_t1[3].label == circuit._one_wire.label);
    REQUIRE(var_t1[2].label == circuit._zero_wire.label);
    REQUIRE(var_t1[1].label == circuit._one_wire.label);
    REQUIRE(var_t1[0].label == circuit._zero_wire.label);
    // > !SECTION - Test 1: Flip variable overriding it.

    // -------------------
    // > SECTION - Test 2: Flip variable into new variable.
    // -------------------
    // This test executes the flip() on the input variable, outputing the result into another variable. This method expects an
    // input variable to flip, and another input variable on which the result will be written.
    // 
    // It is expected that:
    // 1. The input variable is updated with the flipped result;
    // 2. The wires should be mirrored, i.e the most significant wires are converted to least significant wires and vice-versa;
    // 
    // This test includes the following sub-tests: (all the variables have 8 bits)
    // 1. Flip value 15 (00001111). This should result in a variable with value 240/-16 (11110000);
    // 2. Flip value 85 (01010101). This should result in a variable with value 170/-86 (10101010);
    // 3. Flip value 50 (00110010), giving the same input and output variable. This should result in the override of the input
    // variable with value value 76 (01001100);
    Var var_t2(8);
    Var var_t2_out(8);

    // Check flip on value 15
    circuit.assign_value(var_t2, 15);
    circuit.flip(var_t2, var_t2_out);

    // Check input variable
    REQUIRE(var_t2[7].label == circuit._zero_wire.label);
    REQUIRE(var_t2[6].label == circuit._zero_wire.label);
    REQUIRE(var_t2[5].label == circuit._zero_wire.label);
    REQUIRE(var_t2[4].label == circuit._zero_wire.label);
    REQUIRE(var_t2[3].label == circuit._one_wire.label);
    REQUIRE(var_t2[2].label == circuit._one_wire.label);
    REQUIRE(var_t2[1].label == circuit._one_wire.label);
    REQUIRE(var_t2[0].label == circuit._one_wire.label);

    // Check output variable
    REQUIRE(var_t2_out[7].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[6].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[5].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[4].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[3].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[1].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[0].label == circuit._zero_wire.label);

    // Check flip on value 85
    circuit.assign_value(var_t2, 85);
    circuit.flip(var_t2, var_t2_out);
    
    // Check input variable
    REQUIRE(var_t2[7].label == circuit._zero_wire.label);
    REQUIRE(var_t2[6].label == circuit._one_wire.label);
    REQUIRE(var_t2[5].label == circuit._zero_wire.label);
    REQUIRE(var_t2[4].label == circuit._one_wire.label);
    REQUIRE(var_t2[3].label == circuit._zero_wire.label);
    REQUIRE(var_t2[2].label == circuit._one_wire.label);
    REQUIRE(var_t2[1].label == circuit._zero_wire.label);
    REQUIRE(var_t2[0].label == circuit._one_wire.label);

    // Check output variable
    REQUIRE(var_t2_out[7].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[6].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[5].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[4].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[3].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[2].label == circuit._zero_wire.label);
    REQUIRE(var_t2_out[1].label == circuit._one_wire.label);
    REQUIRE(var_t2_out[0].label == circuit._zero_wire.label);

    // Check flip on value 50, giving the same variable as input and output
    circuit.assign_value(var_t2, 50);
    circuit.flip(var_t2, var_t2);
    
    // Check output variable
    REQUIRE(var_t2[7].label == circuit._zero_wire.label);
    REQUIRE(var_t2[6].label == circuit._one_wire.label);
    REQUIRE(var_t2[5].label == circuit._zero_wire.label);
    REQUIRE(var_t2[4].label == circuit._zero_wire.label);
    REQUIRE(var_t2[3].label == circuit._one_wire.label);
    REQUIRE(var_t2[2].label == circuit._one_wire.label);
    REQUIRE(var_t2[1].label == circuit._zero_wire.label);
    REQUIRE(var_t2[0].label == circuit._zero_wire.label);
    // > !SECTION - Test 2: Flip variable into new variable.
    // > !SECTION - Test suit "Flip Operations"
}

TEST_CASE("2's Complement Operations") {
    // > SECTION - Test suit "2's Complement Operations"
    //
    // This test suit aims to test the 2's complement operations of the Circuit Generator class.
    //
    // Note: The constructor of CircuitGenerator does not have a public visibility, thus an instance cannot be created.
    // For the sake of the tests, the tester class makes it accessible.
    //
    // Test suit index:
    // > Test 1: 2's Complement on a variable overriding it
    // > Test 2: 2's Complement on a variable into new variable
    // > !SECTION - Test suit "2's Complement Operations"
}

TEST_CASE("Memory Management") {}
