#pragma once

#include <string>
#include <vector>
#include <fstream>

#include <unistd.h>

namespace gabe {
    namespace circuits {
        namespace generator {
            class CircuitGenerator
            {
            protected:
                std::ofstream _circuit;
                std::string _circuit_path;

                std::fstream _temp_circuit;
                std::string _temp_circuit_path;

                std::vector<uint64_t> _wires_per_input_party;
                std::vector<uint64_t> _wires_per_output_party;

            private:
                void _open_files();
                void _close_files();

            protected:
                CircuitGenerator();
                CircuitGenerator(const std::string &circuit_path, const std::vector<uint64_t>& wires_per_input_party, const std::vector<uint64_t>& wires_per_output_party);

                ~CircuitGenerator();
            };
        }
    }
}
