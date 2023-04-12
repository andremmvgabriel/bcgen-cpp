#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <stdint.h>
#include <filesystem>
#include <unordered_map>

#include <bcgen/variables.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace gabe {
    namespace bcgen {
        class CircuitGenerator
        {
        protected:
            std::string _circuit_name;
            std::filesystem::path _circuits_directory;

            // Circuit info
            std::vector<uint64_t> _input_parties;
            std::vector<uint64_t> _output_parties;
            uint64_t _counter_wires = 0x00;
            uint64_t _counter_gates = 0x00;
            std::unordered_map<std::string, uint64_t> _gates_counters;

            std::unordered_map<std::string, std::string> _gates_map;

            // Zero and One wires
            Wire _zero_wire; // Wire that is always zero in the circuit
            Wire _one_wire; // Wire that is always one in the circuit

            // Control variables
            bool valid = false;
            uint64_t _expected_input_wires = 0x00;
            uint64_t _expected_output_wires = 0x00;

            // Logging
            std::shared_ptr<spdlog::logger> _logger;

            uint64_t _buffer_size = 0x00;
            uint64_t _buffer_max_size = 0xFF;
            std::vector<std::string> _buffer;

        private:
            void _create_save_directory();
            void _setup_logger();

            void _assert_add_input(uint64_t size);

            void _assert_equal_size(const SignedVar& var1, const SignedVar& var2);
            void _assert_equal_size(const UnsignedVar& var1, const UnsignedVar& var2);
            void _assert_equal_size(const SignedVar& var, const uint64_t size);
            void _assert_equal_size(const UnsignedVar& var, const uint64_t size);

            void _flush_buffer(std::ofstream& file);
            void _write_gate(const std::string& line, const std::string& gate);
            void _write_1_1_gate(const uint64_t input, const uint64_t output, const std::string &gate);
            void _write_2_1_gate(const uint64_t input1, const uint64_t input2, const uint64_t output, const std::string &gate);

        protected:
            CircuitGenerator() = delete;
            CircuitGenerator(const std::string &circuit_name);
            CircuitGenerator(const std::string &circuit_name, const std::string &circuits_directory);

            ~CircuitGenerator();

            virtual void _write_header(std::ofstream& file);
            virtual void _write_circuit(std::ofstream& file);

        public:
            // Parties
            void add_input_party(uint64_t size);
            void add_output_party(uint64_t size);

            // Inputs
            void add_input(Wire& wire);
            void add_input(SignedVar& variable);
            void add_input(UnsignedVar& variable);

            // Outputs
            void add_output(Wire& wire);
            void add_output(SignedVar& wire);
            void add_output(UnsignedVar& wire);

            void start();
            void stop();

            // Values assignment
            void assign_value(SignedVar& variable, int64_t value);
            void assign_value(UnsignedVar& variable, uint64_t value);

            // Shifters - Overrides the given variable
            void shift_left(SignedVar& variable, uint64_t amount);
            void shift_left(UnsignedVar& variable, uint64_t amount);
            void shift_right(SignedVar& variable, uint64_t amount);
            void shift_right(UnsignedVar& variable, uint64_t amount);

            // Shifters - Without overriding the given variable
            void shift_left(const SignedVar& variable, uint64_t amount, SignedVar& output);
            void shift_left(const UnsignedVar& variable, uint64_t amount, UnsignedVar& output);
            void shift_right(const SignedVar& variable, uint64_t amount, SignedVar& output);
            void shift_right(const UnsignedVar& variable, uint64_t amount, UnsignedVar& output);

            // Rotators - Overrides the given variable
            void rotate_left(SignedVar& variable, uint64_t amount);
            void rotate_left(UnsignedVar& variable, uint64_t amount);
            void rotate_right(SignedVar& variable, uint64_t amount);
            void rotate_right(UnsignedVar& variable, uint64_t amount);

            // Rotators - Without overriding the given variable
            void rotate_left(const SignedVar& variable, uint64_t amount, SignedVar& output);
            void rotate_left(const UnsignedVar& variable, uint64_t amount, UnsignedVar& output);
            void rotate_right(const SignedVar& variable, uint64_t amount, SignedVar& output);
            void rotate_right(const UnsignedVar& variable, uint64_t amount, UnsignedVar& output);

            // Flipers - Overrides the given variable
            void flip(SignedVar& variable);
            void flip(UnsignedVar& variable);
            
            // Flipers - Without overriding the given variable
            void flip(const SignedVar& variable, SignedVar& output);
            void flip(const UnsignedVar& variable, UnsignedVar& output);

            // Complements - Overrides the given variable
            void twos_complement(SignedVar& variable);
            void twos_complement(UnsignedVar& variable);

            // Complements - Without overriding the given variable
            void twos_complement(const SignedVar& variable, SignedVar& output);
            void twos_complement(const UnsignedVar& variable, UnsignedVar& output);

            // Basic wire operations
            void XOR(const Wire input1, const Wire input2, Wire& output);
            void AND(const Wire input1, const Wire input2, Wire& output);
            void INV(const Wire input, Wire& output);
            void OR(const Wire input1, const Wire input2, Wire& output);

            // Basic signed operations
            void XOR(const SignedVar& input1, const SignedVar& input2, SignedVar& output);
            void AND(const SignedVar& input1, const SignedVar& input2, SignedVar& output);
            void INV(const SignedVar& input, SignedVar& output);
            void OR(const SignedVar& input1, const SignedVar& input2, SignedVar& output);

            // Basic unsigned operations
            void XOR(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);
            void AND(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);
            void INV(const UnsignedVar& input, UnsignedVar& output);
            void OR(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);

            // Arithmetic signed operations
            //void addition(const SignedVar& input1, const SignedVar& input2, SignedVar& output);
            //void subtraction(const SignedVar& input1, const SignedVar& input2, SignedVar& output);
            //void multiplication(const SignedVar& input1, const SignedVar& input2, SignedVar& output);
            //void division(const SignedVar& input1, const SignedVar& input2, SignedVar& output_quotient, SignedVar& output_remainder);
            //void division_quotient(const SignedVar& input1, const SignedVar& input2, SignedVar& output_quotient);
            //void division_remainder(const SignedVar& input1, const SignedVar& input2, SignedVar& output_remainder);

            // Arithmetic unsigned operations
            void addition(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);
            void subtraction(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);
            void multiplication(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);
            void division(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output_quotient, UnsignedVar& output_remainder);
            void division_quotient(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output_quotient);
            void division_remainder(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output_remainder);

            // Conditional signed operations
            void multiplexer(const Wire& control, const SignedVar& input1, const SignedVar& input2, SignedVar& output);
            void multiplexer(const SignedVar& control, const SignedVar& input1, const SignedVar& input2, SignedVar& output);
            void equal(const SignedVar& input1, const SignedVar& input2, Wire& output);
            void equal(const SignedVar& input1, const SignedVar& input2, SignedVar& output);
            //void greater(const SignedVar& input1, const SignedVar& input2, Wire& output);
            //void greater(const SignedVar& input1, const SignedVar& input2, SignedVar& output);
            //void smaller(const SignedVar& input1, const SignedVar& input2, Wire& output);
            //void smaller(const SignedVar& input1, const SignedVar& input2, SignedVar& output);
            //void greater_or_equal(const SignedVar& input1, const SignedVar& input2, Wire& output);
            //void greater_or_equal(const SignedVar& input1, const SignedVar& input2, SignedVar& output);
            //void smaller_or_equal(const SignedVar& input1, const SignedVar& input2, Wire& output);
            //void smaller_or_equal(const SignedVar& input1, const SignedVar& input2, SignedVar& output);
            //void comparator(const SignedVar& input1, const SignedVar& input2, Wire& out_equal, Wire& out_greater, Wire &out_smaller);
            //void comparator(const SignedVar& input1, const SignedVar& input2, SignedVar& out_equal, SignedVar& out_greater, SignedVar &out_smaller);

            // Conditional unsigned operations
            void multiplexer(const Wire& control, const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);
            void multiplexer(const UnsignedVar& control, const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);
            void equal(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output);
            void equal(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);
            void greater(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output);
            void greater(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);
            void smaller(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output);
            void smaller(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);
            void greater_or_equal(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output);
            void greater_or_equal(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);
            void smaller_or_equal(const UnsignedVar& input1, const UnsignedVar& input2, Wire& output);
            void smaller_or_equal(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);
            void comparator(const UnsignedVar& input1, const UnsignedVar& input2, Wire& out_equal, Wire& out_greater, Wire &out_smaller);
            void comparator(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& out_equal, UnsignedVar& out_greater, UnsignedVar &out_smaller);
        };
    }
}
