#include <BristolCircuitGenerator.hpp>
#include <LibscapiCircuitGenerator.hpp>

typedef gabe::circuits::generator::CircuitGenerator circGen;
typedef gabe::circuits::generator::BristolCircuitGenerator bristolGen;
typedef gabe::circuits::generator::LibscapiCircuitGenerator libscapiGen;

typedef gabe::circuits::generator::Wire wire;
typedef gabe::circuits::generator::SignedVar sVar;
typedef gabe::circuits::generator::UnsignedVar uVar;

#define PARTY1_SIZE 8
#define PARTY2_SIZE 8
#define OUTPUT_SIZE 8

void circuit(circGen* generator) {
    // Inputs & Output variables
    uVar input1(PARTY1_SIZE);
    uVar input2(PARTY2_SIZE);
    uVar output(OUTPUT_SIZE);

    // Adding inputs
    generator->add_input(input1);
    generator->add_input(input2);

    // Start the circuit writting
    generator->start();

    // Function / Algorithm
    generator->and(input1, input2, output);

    // Stops the circuit writting
    generator->stop();

    // Adding outputs
    generator->add_output(output);
}

void generate_bristol() {
    bristolGen generator(
        "AND_circuit",
        {PARTY1_SIZE, PARTY2_SIZE},
        {OUTPUT_SIZE}
    );

    circuit(&generator);
}

void generate_libscapi() {
    libscapiGen generator(
        "AND_circuit",
        {8, 8},
        {8}
    );

    circuit(&generator);
}

int main(int argc, char* argv[]) {
    generate_bristol();
    generate_libscapi();

    return 0;
}
