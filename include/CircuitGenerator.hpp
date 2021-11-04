#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <stdint.h>
#include <filesystem>
#include <direct.h>
#include <unordered_map>

#include <Wire.hpp>

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

                // Circuit info
                std::vector<uint64_t> _wires_per_input_party;
                std::vector<uint64_t> _wires_per_output_party;
                //uint64_t _counter_gates = 0x00;
                uint64_t _counter_wires = 0x00;
                uint64_t _counter_xor_gates = 0x00;
                //uint64_t _counter_and_gates = 0x00;
                //uint64_t _counter_inv_gates = 0x00;
                //uint64_t _counter_or_gates = 0x00;

                std::unordered_map<std::string, std::string> _gates_map = {
                    {"xor", ""},
                    {"and", ""},
                    {"inv", ""},
                    {"or", ""}
                };

            private:
                void _create_save_directory();
                void _open_files();
                void _close_files();

                void _write_2_1_gate(const uint64_t in1, const uint64_t in2, const uint64_t out, const std::string &gate);

            public: // After tests put as protected
                CircuitGenerator();
            
                CircuitGenerator(const std::string &circuit_name, const std::vector<uint64_t>& wires_per_input_party, const std::vector<uint64_t>& wires_per_output_party, const std::string &circuits_directory = std::string());

                ~CircuitGenerator();

                void xor(const Wire& in1, const Wire& in2, Wire& out);
            };
        }
    }
}
