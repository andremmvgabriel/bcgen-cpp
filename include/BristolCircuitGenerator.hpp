#pragma once

#include <CircuitGenerator.hpp>

namespace gabe {
    namespace circuits {
        namespace generator {
            class BristolCircuitGenerator : public CircuitGenerator
            {
            protected:
                BristolCircuitGenerator();

                virtual void _write_header() override;
                virtual void _write_header_info() override;
                virtual void _write_header_inputs() override;
                virtual void _write_header_outputs() override;
            
            public:
                BristolCircuitGenerator(const std::string &circuit_name, const std::vector<uint64_t>& wires_per_input_party, const std::vector<uint64_t>& wires_per_output_party, const std::string &circuits_directory = std::string());
            };
        }
    }
}
