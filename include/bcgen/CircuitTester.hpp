#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <unordered_map>

namespace gabe {
    namespace bcgen {
        class CircuitTester
        {
        protected:
            std::ifstream _circuit;
            std::size_t _circuit_start_pos;

            // Circuit info
            std::vector<uint8_t> _wires;
            std::vector<uint64_t> _input_parties; /**<Input parties and their sizes.*/
            std::vector<uint64_t> _output_parties; /**<Output parties and their sizes.*/
            std::unordered_map<std::string, std::function<uint8_t(const std::vector<std::string>&)>> _gates_operations; /**<Mapping of gates operations.*/

            std::vector<std::vector<uint64_t>> _input_parties_wires;
            std::vector<std::vector<uint64_t>> _output_parties_wires;

            // Circuit info complement - Control variables
            uint64_t _counter_wires = 0x00; /**<Control variable to count wires.*/
            uint64_t _counter_gates = 0x00; /**<Control variable to count gates.*/
        
        protected:
            void _print_results();
            void _print_circuit_info();
        
        protected:
            virtual void _read_header() = 0;
            std::vector<std::string> _split_str(std::string string, std::string delimiter);
            CircuitTester() = default;

        public:
            ~CircuitTester();

            void open(const std::string &circuit_name, const std::string& circuit_directory = "circuits");

            void run(const std::vector<uint8_t>& inputs);
        };



        class BristolCircuitTester : public CircuitTester
        {
        protected:
            void _read_header() override;

        public:
            BristolCircuitTester();
        };

        class LibscapiCircuitTester : public CircuitTester
        {
        protected:
            void _read_header() override;

        public:
            LibscapiCircuitTester();
        };
    }
}
