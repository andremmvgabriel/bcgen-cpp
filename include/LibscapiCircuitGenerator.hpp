#pragma once

#include <CircuitGenerator.hpp>

namespace gabe {
    namespace circuits {
        namespace generator {
            class LibscapiCircuitGenerator : public CircuitGenerator
            {
            protected:
                LibscapiCircuitGenerator();
            
            public:
                LibscapiCircuitGenerator(const std::string &circuit_name, const std::vector<uint64_t>& wires_per_input_party, const std::vector<uint64_t>& wires_per_output_party, const std::string &circuits_directory = std::string());
            };
        }
    }
}
