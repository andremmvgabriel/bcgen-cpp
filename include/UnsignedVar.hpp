#pragma once

#include <Variable.hpp>

namespace gabe {
    namespace circuits {
        namespace generator {
            class UnsignedVar : public Variable
            {
            public:
                UnsignedVar();
                UnsignedVar(uint64_t number_wires);
            };
        }
    }
}
