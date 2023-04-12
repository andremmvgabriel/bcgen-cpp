#pragma once

#include <cstdint>
#include <vector>

namespace gabe {
    namespace bcgen {
        struct Wire {
            uint64_t label = 0;
        };

        class Variable
        {
        protected:
            std::vector<Wire> _wires;
        
        protected:
            Variable();
            Variable(uint64_t number_wires);

        public:
            Wire& operator [] (uint64_t index);
            const Wire& operator [] (uint64_t index) const;
            uint64_t size() const;
        };

        class SignedVar : public Variable
        {
        public:
            SignedVar();
            SignedVar(uint64_t number_wires);
        };

        class UnsignedVar : public Variable
        {
        public:
            UnsignedVar();
            UnsignedVar(uint64_t number_wires);
        };
    }
}
