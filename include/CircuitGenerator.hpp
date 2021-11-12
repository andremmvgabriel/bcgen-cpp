#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <stdint.h>
#include <filesystem>
#include <direct.h>
#include <unordered_map>

#include <Wire.hpp>
#include <SignedVar.hpp>
#include <UnsignedVar.hpp>

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
                uint64_t _counter_and_gates = 0x00;
                uint64_t _counter_inv_gates = 0x00;
                uint64_t _counter_or_gates = 0x00;

                std::unordered_map<std::string, std::string> _gates_map = {
                    {"xor", ""},
                    {"and", ""},
                    {"inv", ""},
                    {"or", ""}
                };

                // Zero and One wires
                Wire _zero_wire; // Wire that is always zero in the circuit
                Wire _one_wire; // Wire that is always one in the circuit

            private:
                void _create_save_directory();
                void _open_files();
                void _close_files();

                void _assert_equal_size(const SignedVar& var1, const SignedVar& var2);
                void _assert_equal_size(const UnsignedVar& var1, const UnsignedVar& var2);
                void _assert_equal_size(const SignedVar& var, const uint64_t size);
                void _assert_equal_size(const UnsignedVar& var, const uint64_t size);

                virtual void _write_header();
                virtual void _write_circuit();

                void _write_1_1_gate(const uint64_t input, const uint64_t output, const std::string &gate);
                void _write_2_1_gate(const uint64_t input1, const uint64_t input2, const uint64_t output, const std::string &gate);

            protected:
                CircuitGenerator();
            
                CircuitGenerator(const std::string &circuit_name, const std::vector<uint64_t>& wires_per_input_party, const std::vector<uint64_t>& wires_per_output_party, const std::string &circuits_directory = std::string());

                ~CircuitGenerator();

            public:
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
                void xor(const Wire& input1, const Wire& input2, Wire& output);
                void and(const Wire& input1, const Wire& input2, Wire& output);
                void inv(const Wire& input, Wire& output);
                void or(const Wire& input1, const Wire& input2, Wire& output);

                // Basic signed operations
                void xor(const SignedVar& input1, const SignedVar& input2, SignedVar& output);
                void and(const SignedVar& input1, const SignedVar& input2, SignedVar& output);
                void inv(const SignedVar& input, SignedVar& output);
                void or(const SignedVar& input1, const SignedVar& input2, SignedVar& output);

                // Basic unsigned operations
                void xor(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);
                void and(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);
                void inv(const UnsignedVar& input, UnsignedVar& output);
                void or(const UnsignedVar& input1, const UnsignedVar& input2, UnsignedVar& output);

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
}
