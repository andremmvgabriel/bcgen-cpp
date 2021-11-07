#pragma once

#include <Variable.hpp>

namespace gabe {
    namespace circuits {
        namespace generator {
            class SignedVar : public Variable
            {
            public:
                SignedVar();
                SignedVar(uint64_t number_wires);
            };
        }
    }
}
