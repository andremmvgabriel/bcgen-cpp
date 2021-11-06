#pragma once

#include <stdint.h>
#include <vector>

#include <Wire.hpp>

namespace gabe {
    namespace circuits {
        namespace generator {
            class SignedVar
            {
            public:
                SignedVar();
                SignedVar(uint64_t number_wires);

                uint64_t number_wires = 0;
                std::vector<Wire> wires;
            };
        }
    }
}
