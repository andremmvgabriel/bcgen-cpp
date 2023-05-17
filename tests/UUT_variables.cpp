#include <bcgen/CircuitGenerator.hpp>
#include <catch2/catch_test_macros.hpp>

// Don't do this :)
using Wire = gabe::bcgen::Wire;
using Var = gabe::bcgen::Variable;

TEST_CASE( "Wire" ) {
    // Wire is just a struct with a single value. It should not require a test.
    // Anyhow... Test cases:
    // 1. Default wire construction
    // 2. Wire construction with label attribution

    SECTION( "Default wire construction" ) {
        Wire wire;
        REQUIRE(wire.label == 0);
    }

    SECTION( "Wire construction with label attribution" ) {
        Wire wire{2};
        REQUIRE(wire.label == 2);
    }
}

TEST_CASE( "Variable" ) {
    // Wire is just a struct with a single value. It should not require a test.
    // Anyhow... Test cases:
    // 1. Default wire construction
    // 2. Wire construction with label attribution

    // SECTION( "Default variable construction" ) {
    //     Var var;
    //     // REQUIRE(wire.label == 0);
    // }
}
