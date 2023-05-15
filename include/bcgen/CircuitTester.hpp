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
             * The following is an example of the information that is printed into the terminal:
             * 
             * ```
             * +--------------------------------------------------------------------------------+
    	     * + Name: circuit name                                                             +
    	     * + Path: directory path that contains the circuit file                            +
    	     * +                                                                                +
    	     * + Total gates: 47                                                                +
    	     * + Total wires: 63                                                                +
    	     * +                                                                                +
    	     * +  Party   +           Inputs sizes           +           Output sizes           +
    	     * +    0     |                8                 |                8                 +
    	     * +    1     |                8                 |                -                 +
    	     * +--------------------------------------------------------------------------------+
             * ```
            **/
            void _print_circuit_info();

            /**
             * @brief Prints the circuit execution results to the user.
             * 
             * -----
             * 
             * Once called, this function writes in the terminal/console the execution result of the circuit
             * execution.
             * 
             * The following is an example of the information that is printed into the terminal:
             * 
             * ```
    	     * +--------------------------------------------------------------------------------+
             * +                              >>> Execution 1 <<<                               +
    	     * + Inputs:                                                                        +
    	     * +--> P0    : 01010100                                                            +
    	     * +--> P1    : 01001000                                                            +
    	     * + Outputs:                                                                       +
    	     * +--> P0    : 00111100                                                            +
    	     * +--------------------------------------------------------------------------------+
             * ```
            **/
            void _print_results();

            /**
             * @brief Reads the header section of the circuit file.
             * 
             * -----
             * 
             * Each circuit file contains a header section that has a particular format for each type. Thus this function is pure
             * virtual and every child class has to overrite this function. It is designed to contain the reading format for the
             * specific format that it is targetting.   
            **/
            virtual void _read_header() = 0;

            /**
             * @brief Splits a string into multiple parts by a delimiter.
             * 
             * -----
             * 
             * This function searches all the delimiters that the input word contains and divides it in those points.
             * If no delimiter is found, the vector should contain a single element which is the input word.
             * 
             * @param word String to split.
             * @param delimiter Delimiter to split at.
             * @return List with all the word parts.
            **/
            std::vector<std::string> _split_str(std::string word, std::string delimiter);

            /**
             * @brief Construct a new Circuit Tester object.
             * @note This is protected to avoid the instantiation of the object.
            **/
            CircuitTester() = default;

        public:
            /** @brief Destroy the Circuit Tester object. **/
            ~CircuitTester();

            /**
             * @brief Opens a specified circuit file.
             * 
             * @note By default, the directory to the circuits is located at a circuits directory that should be in the same level
             * from where the executable is being executed.
             * 
             * @param circuit_name Circuit file name.
             * @param circuit_directory Directory to search the circuit file at.
            **/
            void open(const std::string &circuit_name, const std::string& circuit_directory = "circuits");

            /**
             * @brief Executes the circuit with the input values.
             * @note The inputs should be given with from least significant wires (lower indexes in the list) to the most
             * significant wires (higher indexes in the list)
             * @param inputs Input values.
            **/
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
            /**
             * @brief Reads the header section of the Bristol circuit file.
             * 
             * -----
             * 
             * This function contains the header reading for Bristol circuit formats.
            **/
            void _read_header() override;

        public:
            /** @brief Construct a new Bristol Circuit Tester object. **/
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
            /**
             * @brief Reads the header section of the Libscapi circuit file.
             * 
             * -----
             * 
             * This function contains the header reading for Libscapi circuit formats.
            **/
            void _read_header() override;

        public:
            /** @brief Construct a new Libscapi Circuit Tester object. **/
            LibscapiCircuitTester();
        };
    }
}
