#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <stdint.h>
#include <filesystem>
#include <direct.h>

namespace gabe {
    namespace circuits {
        namespace generator {
            class CircuitGenerator
            {
            protected:
                std::string _circuits_directory;

                std::ofstream _circuit;
                std::string _circuit_name;

                std::fstream _temp_circuit;
                std::string _temp_circuit_name;

                std::vector<uint64_t> _wires_per_input_party;
                std::vector<uint64_t> _wires_per_output_party;

            private:
                void _create_save_directory();
                void _open_files();
                void _close_files();

            protected:
                CircuitGenerator();
            
            public:
                CircuitGenerator(const std::string &circuit_name, const std::vector<uint64_t>& wires_per_input_party, const std::vector<uint64_t>& wires_per_output_party, const std::string &circuits_directory = std::string());

                ~CircuitGenerator();
            };
        }
    }
}
