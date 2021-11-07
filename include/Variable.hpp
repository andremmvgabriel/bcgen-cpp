#pragma once

#include <stdint.h>
#include <vector>

#include <Wire.hpp>

namespace gabe {
    namespace circuits {
        namespace generator {
            class Variable
            {
            protected:
                std::vector<Wire> wires;
            
            protected:
                Variable();
                Variable(uint64_t number_wires);

            public:
                Wire& operator [] (uint64_t index);
                uint64_t size();
            };
        }
    }
}
