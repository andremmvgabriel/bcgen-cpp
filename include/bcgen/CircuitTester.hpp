#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <unordered_map>

namespace gabe {
    namespace bcgen {
        /**
         * @brief Circuit tester class.
         * 
         * -----
         * 
         * This class has the core functionality of a circuit tester. Every class that extends from this will inherite this
         * core methods. The child classes can also overwrite two methods that should be changed for specialized purposes.
         * 
         * @note Currently, this class is limited to integer operations.
         * 
         * @note This class is abstract and designed to not be able to be instantiated. It is meant to be extended from, and its
         * child dedicated to a circuit format.
        **/
        class CircuitTester
        {
        protected:
            // Circuit name and location
            std::string _circuit_name; /**<Circuit file name.*/
            std::string _circuits_directory; /**<Circuit location.*/

            // Circuit file
            std::ifstream _circuit; /**<Circuit file.*/
            std::size_t _circuit_start_pos; /**<Start position (reader pointer) of the circuit section.*/

            // Circuit info
            std::vector<uint8_t> _wires; /**<Circuit wires values.*/
            std::vector<uint64_t> _input_parties; /**<Input parties and their sizes.*/
            std::vector<uint64_t> _output_parties; /**<Output parties and their sizes.*/
            std::vector<std::vector<uint64_t>> _input_parties_wires; /**<Input parties wire labels.*/
            std::vector<std::vector<uint64_t>> _output_parties_wires; /**<Output parties wire labels.*/
            std::unordered_map<std::string, std::function<uint8_t(const std::vector<std::string>&)>> _gates_operations; /**<Mapping of gates operations.*/

            // Circuit info complement - Control variables
            uint64_t _counter_wires = 0x00; /**<Control variable to count wires.*/
            uint64_t _counter_gates = 0x00; /**<Control variable to count gates.*/

            // Other control variables
            uint64_t _counter_executions = 0x00; /**<Control variable to count circuit executions.*/
        
        protected:
            /**
             * @brief Prints the circuit information to the user.
             * 
             * -----
             * 
             * Once called, this function writes in the terminal/console the information that was read from the circuit file
             * header section.
             * 
             * 
            **/
            void _print_circuit_info();
        
            void _print_results();

        protected:
            virtual void _read_header() = 0;
            std::vector<std::string> _split_str(std::string string, std::string delimiter);
            CircuitTester() = default;

        public:
            ~CircuitTester();

            void open(const std::string &circuit_name, const std::string& circuit_directory = "circuits");

            void run(const std::vector<uint8_t>& inputs);
        };

        /**
         * @brief Circuit tester class for Bristol format circuits.
         * 
         * -----
         * 
         * This class extends all the core functionality of a circuit tester abstract class and it was developed to target the
         * testing of boolean circuits with [Bristol](https://homes.esat.kuleuven.be/~nsmart/MPC/) format (check link to
         * undestand the specific format features for Bristol).
        **/
        class BristolCircuitTester : public CircuitTester
        {
        protected:
            void _read_header() override;

        public:
            BristolCircuitTester();
        };

        /**
         * @brief Circuit tester class for Libscapi format circuits.
         * 
         * -----
         * 
         * This class extends all the core functionality of a circuit tester abstract class and it was developed to target the
         * testing of boolean circuits with [Libscapi](https://libscapi.readthedocs.io/en/latest/circuits.html) format
         * (check link to undestand the specific format features for Libscapi).
         * 
         * @note Libscapi is an open source secure computation library that has it's own format to write/read boolean circuits.
         * Here is their [github](https://github.com/cryptobiu/libscapi).
        **/
        class LibscapiCircuitTester : public CircuitTester
        {
        protected:
            void _read_header() override;

        public:
            LibscapiCircuitTester();
        };
    }
}
