#pragma once

#include <bcgen/CircuitGenerator.hpp>

#include <string>
#include <vector>
#include <fstream>

namespace gabe {
    namespace bcgen {
        class LibscapiCircuitGenerator : public CircuitGenerator
        {
        protected:
            virtual void _write_header(std::ofstream& file) override;
        
        public:
            LibscapiCircuitGenerator() = delete;
            LibscapiCircuitGenerator(const std::string &circuit_name);
            LibscapiCircuitGenerator(const std::string &circuit_name, const std::string &circuits_directory);
        };
    }   
}
