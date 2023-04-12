#pragma once

#include <bcgen/CircuitGenerator.hpp>

#include <string>
#include <vector>

namespace gabe {
    namespace bcgen {
        class BristolCircuitGenerator : public CircuitGenerator
        {
        protected:
            virtual void _write_header(std::ofstream& file) override;
        
        public:
            BristolCircuitGenerator() = delete;
            BristolCircuitGenerator(const std::string &circuit_name);
            BristolCircuitGenerator(const std::string &circuit_name, const std::string &circuits_directory);
        };
    }   
}
