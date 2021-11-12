#pragma once

#include <CircuitGenerator.hpp>

namespace gabe {
    namespace circuits {
        namespace generator {
            class BristolCircuitGenerator : public CircuitGenerator
            {
            protected:
                BristolCircuitGenerator();
            
            public:
                BristolCircuitGenerator(const std::string &circuit_name, const std::vector<uint64_t>& wires_per_input_party, const std::vector<uint64_t>& wires_per_output_party, const std::string &circuits_directory = std::string());
            };
        }
    }
}
