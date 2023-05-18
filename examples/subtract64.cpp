#include <bcgen/CircuitGenerator.hpp>

typedef gabe::bcgen::CircuitGenerator cGen;
typedef gabe::bcgen::BristolCircuitGenerator bGen;
typedef gabe::bcgen::LibscapiCircuitGenerator lGen;

typedef gabe::bcgen::Wire Wire;
typedef gabe::bcgen::Variable Var;

#define PARTY1_SIZE 64
#define PARTY2_SIZE 64
#define OUTPUT_SIZE 64

void circuit(cGen* generator) {
    // Parties
    generator->add_input_party(PARTY1_SIZE);
    generator->add_input_party(PARTY2_SIZE);
    generator->add_output_party(OUTPUT_SIZE);

    // Inputs
    Var in_1(PARTY1_SIZE);
    Var in_2(PARTY2_SIZE);
    generator->add_input(in_1);
    generator->add_input(in_2);

    // Outputs
    Var out(OUTPUT_SIZE);
    generator->add_output(out);

    // Start the circuit writting
    generator->start();

    // Function / Algorithm
    generator->subtract(in_1, in_2, out);

    // Stops the circuit writting
    generator->stop();
}

int main(int argc, char* argv[]) {
    bGen bgen( "Subtract64", "circuits/examples" ); circuit( &bgen );
    lGen lgen( "Subtract64", "circuits/examples" ); circuit( &lgen );

    return 0;
}
