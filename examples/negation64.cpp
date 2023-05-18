#include <bcgen/CircuitGenerator.hpp>

typedef gabe::bcgen::CircuitGenerator cGen;
typedef gabe::bcgen::BristolCircuitGenerator bGen;
typedef gabe::bcgen::LibscapiCircuitGenerator lGen;

typedef gabe::bcgen::Wire Wire;
typedef gabe::bcgen::Variable Var;

#define PARTY1_SIZE 64
#define OUTPUT_SIZE 64

void circuit(cGen* generator) {
    // Parties
    generator->add_input_party(PARTY1_SIZE);
    generator->add_output_party(OUTPUT_SIZE);

    // Inputs
    Var in_1(PARTY1_SIZE);
    generator->add_input(in_1);

    // Outputs
    Var out(OUTPUT_SIZE);
    generator->add_output(out);

    // Start the circuit writting
    generator->start();

    // Function / Algorithm
    generator->INV(in_1, out);

    // Stops the circuit writting
    generator->stop();
}

int main(int argc, char* argv[]) {
    bGen bgen( "Negation64", "circuits/examples" ); circuit( &bgen );
    lGen lgen( "Negation64", "circuits/examples" ); circuit( &lgen );

    return 0;
}
