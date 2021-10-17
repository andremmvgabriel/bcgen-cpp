#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE UUT_CircuitGeneratorModule
#include <boost/test/included/unit_test.hpp>

#include <CircuitGenerator.hpp>

// ----- 

const std::string circuits_directory = "unit_tests/UUT_CircuitGenerator_Circuits/";

struct Fixture {
    Fixture() {
        mkdir( circuits_directory.c_str(), 07777);
    }

    ~Fixture() {}
};

BOOST_GLOBAL_FIXTURE(Fixture);

// -----

class UUT_CircuitGenerator : public gabe::circuits::generator::CircuitGenerator {
public:
    using gabe::circuits::generator::CircuitGenerator::_circuit;
    using gabe::circuits::generator::CircuitGenerator::_circuit_path;

    using gabe::circuits::generator::CircuitGenerator::_temp_circuit;
    using gabe::circuits::generator::CircuitGenerator::_temp_circuit_path;

    using gabe::circuits::generator::CircuitGenerator::_wires_per_input_party;
    using gabe::circuits::generator::CircuitGenerator::_wires_per_output_party;

public:
    UUT_CircuitGenerator(const std::string &circuit_path, const std::vector<uint64_t>& wires_per_input_party, const std::vector<uint64_t>& wires_per_output_party) : CircuitGenerator(circuit_path, wires_per_input_party, wires_per_output_party) {}
};

// -----

BOOST_AUTO_TEST_SUITE(Base);

BOOST_AUTO_TEST_CASE(InitializeAndDestroyGenerator) {
    // Setup
    std::string circuit_path = circuits_directory + "InitializeAndDestroyGenerator.txt";
    std::string temp_circuit_path = circuits_directory + "InitializeAndDestroyGenerator_temp.txt";
    std::vector<uint64_t> inputs = {2, 4, 6, 8};
    std::vector<uint64_t> outputs = {1, 2};

    /**
     * Tests while Circuit Generator is "alive"
     */
    {
        UUT_CircuitGenerator generator(
            circuit_path,
            inputs,
            outputs
        );

        // Main circuit file
        BOOST_CHECK(generator._circuit_path == circuit_path);
        BOOST_CHECK(generator._circuit.is_open());

        // Temporary circuit file
        BOOST_CHECK(generator._temp_circuit_path == temp_circuit_path);
        BOOST_CHECK(generator._temp_circuit.is_open());

        // Inputs & Outputs size
        BOOST_CHECK(generator._wires_per_input_party.size() == inputs.size());
        BOOST_CHECK(generator._wires_per_output_party.size() == outputs.size());

        // Wires per input party
        for (int i = 0; i < inputs.size(); i++)
            BOOST_CHECK(generator._wires_per_input_party[i] == inputs[i]);

        // Wires per output party
        for (int i = 0; i < outputs.size(); i++)
            BOOST_CHECK(generator._wires_per_output_party[i] == outputs[i]);
    }

    /**
     * Tests while Circuit Generator is no longer "alive"
     */

    // Opens both main and temporary files
    std::fstream circuit( circuit_path, std::ios::in | std::ios::out );
    std::fstream temp_circuit( temp_circuit_path, std::ios::in | std::ios::out );

    // Main circuit file should exist. Temporary one should not.
    BOOST_CHECK(circuit.is_open());
    BOOST_CHECK(!temp_circuit.is_open());
}

BOOST_AUTO_TEST_SUITE_END();
