#pragma once

#include <stdint.h>
#include <vector>

#include <Wire.hpp>

namespace gabe {
    namespace circuits {
        namespace generator {
            class UnsignedVariable
            {
            public:
                UnsignedVariable();
                UnsignedVariable(uint64_t number_wires);

                uint64_t number_wires = 0;
                std::vector<Wire> wires;
            };
        }
    }
}
