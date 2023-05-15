#pragma once

#include <numeric>
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <filesystem>
#include <unordered_map>

namespace gabe {
    namespace bcgen {
        /** @brief Wire structure. **/
        struct Wire {
            uint64_t label = 0; /**<Wire label.*/
        };

        /**
         * @brief Variable class.
         * 
         * -----
         * 
         * This class has a vector that contains wires. The less the index in the vector, the least significant the wire is.
         * 
         * @note A variable object cannot be instantiated without specifying a size.
        **/
        class Variable
        {
        private:
            // QoL typedefs for iterator component
            using iterator = std::vector<Wire>::iterator;
            using const_iterator = std::vector<Wire>::const_iterator;

        protected:
            std::vector<Wire> _wires; /**<Variable wires.*/
        
        public:
            /** @brief Default construction of a new Varaible object is deleted. **/
            Variable() = delete;

            /**
             * @brief Construct a new Variable object.
             * 
             * -----
             * 
             * Creates a new object of type Variable with a specified number of wires.
             * 
             * @param number_wires Variable size.
            **/
            Variable(uint64_t number_wires);

        public:
            /**
             * @brief Accesses a wire positioned in an input index.
             * 
             * -----
             * 
             * This method allows the mutation of wire value that is being accessed. 
             * 
             * @param index Wire index.
             * @return Accessed wire.
            **/
            Wire& operator [] (uint64_t index);

            /**
             * @brief Accesses a wire positioned in an input index.
             * 
             * -----
             * 
             * This method does allows the mutation of wire value that is being accessed. 
             * 
             * @param Wire index.
             * @return Accessed wire.
            **/
            const Wire& operator [] (uint64_t index) const;

            /**
             * @brief Get the size of the variable.
             * @return Size of the variable (number wires).
            **/
            uint64_t size() const;

            /**
             * @brief Iteration begin method.
             * @note This iteration is mutable.
             * @return Iterator with initial position.
            **/
            iterator begin() { return _wires.begin(); }

            /**
             * @brief Iteration end method.
             * @return Iterator after final position.
            **/
            iterator end() { return _wires.end(); }

            /**
             * @brief Constant iteration begin method.
             * @note This iteration is not mutable.
             * @return Constant iterator with initial position.
            **/
            const_iterator begin() const { return _wires.begin(); }

            /**
             * @brief Constant iteration end method.
             * @return Constant iterator after final position.
            **/
            const_iterator end() const { return _wires.end(); }
        };
        
        /**
         * @brief Circuit generator class.
         * 
         * -----
         * 
         * This class has the core functionality of a circuit generator. Every class that extends from this will inherite this
         * core methods. The child classes can also overwrite two methods that should be changed for specialized purposes.
         * 
         * @note Currently, this class is limited to integer operations.
         * 
         * @note This class is abstract and designed to not be able to be instantiated. It is meant to be extended from, and its
         * child dedicated to a circuit format.
        **/
        class CircuitGenerator
        {
        protected:
            // Circuit name and location
            std::string _circuit_name; /**<Circuit file name.*/
            std::filesystem::path _circuits_directory; /**<Circuit location.*/

            // Circuit info
            std::vector<uint64_t> _input_parties; /**<Input parties and their sizes.*/
            std::vector<uint64_t> _output_parties; /**<Output parties and their sizes.*/
            std::vector<Wire*> _output_wires; /**<Output wires from the output parties.*/
            std::unordered_map<std::string, std::string> _gates_map; /**<Mapping of gates names.*/

            // Circuit info complement - Control variables
            uint64_t _counter_wires = 0x00; /**<Control variable to count wires.*/
            uint64_t _counter_gates = 0x00; /**<Control variable to count gates.*/
            uint64_t _expected_input_wires = 0x00; /**<Input wires expected to be assigned to input variables.*/
            uint64_t _expected_output_wires = 0x00; /**<Output wires expected to be assigned to output variables.*/
            uint64_t _assigned_output_wires = 0x00; /**<Output wires expected to be assigned to output variables.*/
            std::unordered_map<std::string, uint64_t> _gates_counters; /**<Mapping control variable to count all the gates.*/

            // Circuit buffer - Memory management
            uint64_t _buffer_size = 0x00; /**<Control variable to control the current size of the buffer.*/
            uint64_t _buffer_max_size = std::numeric_limits<uint64_t>::max(); /**<Maximum size of the buffer.*/
            std::vector<std::string> _buffer; /**<Buffer to write the circuit in.*/

            // Zero and One wires
            Wire _zero_wire; /**<Wire that is always zero in the circuit.*/
            Wire _one_wire; /**<Wire that is always one in the circuit.*/

        // Internal Core Methods
        private:
            /**
             * @brief Creates the directory on which the circuit file will be generated.
             * 
             * -----
             * 
             * By default, the circuits location is directed to a "circuits" directory in the same directory on which the
             * executable is called.
             *
             * @note It will not create the directory if the specified string is empty.
            **/
            void _create_save_directory();

            /**
             * @brief Flushes the buffer contents into a file.
             * 
             * -----
             * 
             * This function iterates through all the strings that the buffer contains and writes them into the input file.
             * The buffer is then emptied and the buffer size control variable restarted to 0.
             * 
             * @param file File on which the buffer contents will be written.
            **/
            void _flush_buffer(std::ofstream& file);

            /**
             * @brief Writes a gate line of the circuit.
             * 
             * -----
             * 
             * This function performs multiple steps:
             * 1. Adds the line into the circuit buffer;
             * 2. Checks if the buffer should be flushed. Flushes if so;
             * 3. Updates the counters.
             * 
             * @param line Gate line.
             * @param gate Gate name.
            **/
            void _write_gate(const std::string& line, const std::string& gate);

            /**
             * @brief Writes a logic gate that has 1 input wire and 1 output wire.
             * 
             * -----
             * 
             * This function only constructs the gate (1:1) line that will be written into the circuit file. This line is then
             * given to the _write_gate function.
             * 
             * @param in_a Input wire.
             * @param output Output wire.
             * @param gate Gate name.
            **/
            void _write_1_1_gate(const uint64_t in_a, const uint64_t output, const std::string &gate);

            /**
             * @brief Writes a logic gate that has 2 input wires and 1 output wire.
             * 
             * -----
             * 
             * This function only constructs the gate (2:1) line that will be written into the circuit file. This line is then
             * given to the _write_gate function.
             * 
             * @param in_a Input wire A.
             * @param in_b Input wire B.
             * @param output Output wire.
             * @param gate Gate name.
            **/
            void _write_2_1_gate(const uint64_t in_a, const uint64_t in_b, const uint64_t output, const std::string &gate);

        // Assertions | Errors | Exceptions
        private:
            /**
             * @brief Checks if a variable has a specific size.
             * 
             * -----
             * 
             * This check raises a runtime expection if the variable does not meet the required size.
             * 
             * @param variable Variable to check.
             * @param size Size to evaluate.
            **/
            void _assert_equal_size(const Variable& variable, uint64_t size);

            /**
             * @brief Checks if the input variable can be added as input.
             * 
             * -----
             * 
             * This check raises a runtime expection if the variable size does exceed the total of available input wires.
             * 
             * @param size Number of wires to evaluate.
            **/
            void _assert_add_input(uint64_t size);

            /**
             * @brief Checks if the output variable can be added as output.
             * 
             * -----
             * 
             * This check raises a runtime expection if the variable size does exceed the total of available output wires.
             * 
             * @param size Number of wires to evaluate.
            **/
            void _assert_add_output(uint64_t size);
        
        // Constructors | Destructor
        protected:
            /** @brief Default construction of a new Circuit Generator object is deleted. **/
            CircuitGenerator() = delete;

            /**
             * @brief Construct a new Circuit Generator object for a specified circuit name and location.
             * 
             * -----
             * 
             * In the end, the generated circuit file will have the name of the inserted circuit name and located into the
             * inserted location.
             * 
             * @note By default, the directory to the circuits is located at a circuits directory that should be in the same level
             * from where the executable is being executed.
             * 
             * @param circuit_name Name of the circuit.
             * @param circuits_directory Location of the circuit.
            **/
            CircuitGenerator(const std::string &circuit_name, const std::string &circuits_directory = "circuits");

            /** @brief Destroy the Circuit Generator object. **/
            ~CircuitGenerator();

        // Abstract Methods
        protected:
            /**
             * @brief Writes the header section of the circuit into a file.
             * 
             * -----
             * 
             * By default, this function is empty. It is meant to be overritten by any circuit generator class that extends from
             * this one.
             * 
             * @param file File to write.
            **/
            virtual void _write_header(std::ofstream& file);

            /**
             * @brief Writes the circuit section of the circuit into a file.
             * 
             * -----
             * 
             * By default, this function is performs the write of the circuit section exactly how it should be. However, it is
             * placed as abstract in case any child class needs to change its behavior.
             * 
             * @param file File to write.
            **/
            virtual void _write_circuit(std::ofstream& file);

        // Generator Options
        public:
            /**
             * @brief Sets a buffer size limit to the internal buffer.
             * 
             * -----
             * 
             * By default, the circuit generator caches all the generated data until it is written into a circuit file, which is
             * the last step. However, this can be a problem in several machines that are more RAM limited. In those cases, this
             * function can be used to set a buffer limit to write into the circuit file once the buffer reaches a specified size.
             * The size is in bytes.
             * 
             * @param size Size of the buffer.
            **/
            void limit_buffer(uint64_t size);

        // Circuit Setup
        public:
            /**
             * @brief Adds an input party to the circuit.
             * 
             * -----
             * 
             * This function adds a new input party and registers its size into a vector of input parties. The inserted party size
             * will increase the number of available input variables that can be used to add input variables. This size is also
             * important in the writing phase.
             * 
             * @note This function should only be used before starting the circuit writing (after using the start() method).
             * Otherwise, this function will not behave as expected.
             * 
             * @param size Size of the input party.
            **/
            void add_input_party(uint64_t size);

            /**
             * @brief Adds an output party to the circuit.
             * 
             * -----
             * 
             * This function adds a new output party and registers its size into a vector of output parties.
             * 
             * @note This function should only be used before starting the circuit writing (after using the start() method).
             * Otherwise, this function will not behave as expected.
             * 
             * @param size Size of the output party.
            **/
            void add_output_party(uint64_t size);

            /**
             * @brief Adds a wire as a circuit input.
             * 
             * -----
             * 
             * This will assign a label to the inputed wire.
             * This function expects that there is at least one avaliable input wire to be assigned into the input wire. The
             * amount of circuit input wires is manageable by the add_input_party function.
             * In case there is not an available wire, this function will raise a runtime exception.
             * 
             * @note This function should only be used before starting the circuit writing (before using the start() method).
             * Otherwise, this function will not behave as expected.
             * 
             * @param wire Input wire in the circuit.
            **/
            void add_input(Wire& wire);

            /**
             * @brief Adds a variable as a circuit input.
             * 
             * -----
             * 
             * This will assign labels to all the wires inside the inputed variable.
             * This function expects that there are enough avaliable input wires to be assigned into the input variable. The
             * amount of circuit input wires is manageable by the add_input_party function.
             * In case there are not enough input wires, this function will raise a runtime exception.
             * 
             * @note This function should only be used before starting the circuit writing (before using the start() method).
             * Otherwise, this function will not behave as expected.
             * 
             * @param variable Input variable in the circuit.
            **/
            void add_input(Variable& variable);

            /**
             * @brief Adds a wire as a circuit output.
             * 
             * -----
             * 
             * This will cache the input wire in a class member, which will be used in the very last step of the circuit
             * writing (stop method).
             * This function expects that there is at least one avaliable output wire. The amount of circuit output wires is
             * manageable by the add_output_party function.
             * In case there is not an available expected output wire, this function will raise a runtime exception.
             * 
             * Unlike the add_input function, this one will not assign a label to the inputed wire.
             * 
             * @note This function should only be used before starting the circuit writing (before using the start() method).
             * Otherwise, this function will not behave as expected.
             * 
             * @param wire Output wire in the circuit.
            **/
            void add_output(Wire& wire);

            /**
             * @brief Adds a variable as a circuit output.
             * 
             * -----
             * 
             * This will cache the input wire in a class member, which will be used in the very last step of the circuit
             * writing (stop method).
             * This function expects that there is at least one avaliable output wire. The amount of circuit output wires is
             * manageable by the add_output_party function.
             * In case there is not an available expected output wire, this function will raise a runtime exception.
             * 
             * Unlike the add_input function, this one will not assign a label to the inputed wire.
             * 
             * @note This function should only be used before starting the circuit writing (before using the start() method).
             * Otherwise, this function will not behave as expected.
             * 
             * @param variable Output variable in the circuit.
            **/
            void add_output(Variable& variable);

            /**
             * @brief Starts the writing/construction of the circuit.
             * 
             * -----
             * 
             * This function makes sure that all the inputs of the circuit were correctly setup, i.e that there are inputs and
             * that all the defined input wires were all assigned to input variables.
             * If something is wrong with the initial setup, this function raises a runtime exception.
             * 
             * Once the validation checks pass, this function ends up by creating the zero and one wires, wich are constant wires
             * that assume the value 0 and 1 in the circuit, respectively.
             * 
             * @note The add_input and add_input_party methods should not be used once this function is called.
            **/
            void start();

            /**
             * @brief Stops the writing/construction of the circuit.
             * 
             * -----
             * 
             * Once called, this function terminates the circuit construction and completes the writing into the circuit file,
             * including defining correctly its header part, which is dependent of the total gates in the circuit.
             * To conclude, it also prints into the console the total amount of gates and wires of the created circuit.
            **/
            void stop();

        // Variables manipulation
        public:
            /**
             * @brief Assigns a value to a inputed wire.
             * 
             * -----
             * 
             * A wire can have a value either 0 or 1. Whenever it is needed to have a wire with one of these constants, this
             * function should be used.
             * Once called, the wire label is filled as the "zero_wire" or as the "one_wire", depending if the desired value is 0 
             * or 1, respectively.
             * 
             * This function supports a 8 bit value as input, however, only the least signiticant bit is taken into account. For
             * example, if the user inputs a value of 254 (11111110), the wire is assumed to have the value of 0.
             * 
             * @note This function should only be used after starting the circuit writing (after using the start() method).
             * Otherwise, this function will not behave as expected.
             * 
             * @param wire Wire reference to assign a value to.
             * @param value Value that the wire will have.
            **/
            void assign_value(Wire& wire, uint8_t value);

            /**
             * @brief Assigns a value to a inputed variable.
             * 
             * -----
             * 
             * A variable can have any value the user wants, as long as it have enough wires to assign values to.
             * Whenever it is needed to have a variable with a constant value, this function should be used.
             * Once called, each variable wire will be filled as the "zero_wire" or as the "one_wire", depending if the value bit
             * is 0 or 1, respectively.
             * 
             * This function supports a 64 bit value as input, but the variable does not have to have 64 bits. Only the least
             * signiticant bits is taken into account.
             *
             * @note This function should only be used after starting the circuit writing (after using the start() method).
             * Otherwise, this function will not behave as expected.
             * 
             * @param variable Variable reference to assign a value to.
             * @param value Value that the variable will have.
            **/
            void assign_value(Variable& variable, uint64_t value);

            /**
             * @brief Shift variable wires to the left. Output overrides the input variable.
             * 
             * -----
             * 
             * This function shifts the variable wires towards the left. This means that each wire is increasing its significance
             * by X, being X the amount of bits to shift.
             * The next wires that appear on the right are automatically assigned to have a wire value of 0 (zero_wire of the
             * circuit).
             * 
             * The following example shows the expected behavior of the function:
             * 
             * ```
             * Input variable value: 13
             * Shifting amount: 3
             * 
             *          Shifting
             * 00001101 -------> 01101000
             * ```
             * 
             * @param variable Input & Output variable.
             * @param amount Amount of bits to shift.
            **/
            void shift_left(Variable& variable, uint64_t amount);

            /**
             * @brief Shift variable wires to the left. Output does not override initial variable.
             * 
             * -----
             * 
             * This function shifts the variable wires towards the left. This means that each wire is increasing its significance
             * by X, being X the amount of bits to shift.
             * The next wires that appear on the right are automatically assigned to have a wire value of 0 (zero_wire of the
             * circuit).
             * 
             * The following example shows the expected behavior of the function:
             * 
             * ```
             * Input variable value: 13
             * Shifting amount: 3
             * 
             *          Shifting
             * 00001101 -------> 01101000
             * ```
             * 
             * @param variable Input & Output variable.
             * @param amount Amount of bits to shift.
             * @param output Shifted output.
            **/
            void shift_left(const Variable& variable, uint64_t amount, Variable& output);

            /**
             * @brief Shift variable wires to the right. Output overrides the input variable.
             * 
             * -----
             * 
             * This function shifts the variable wires towards the right. This means that each wire is decreasing its significance
             * by X, being X the amount of bits to shift.
             * The next wires that appear on the left are automatically assigned to have a wire value of 0 (zero_wire of the
             * circuit).
             * 
             * The following example shows the expected behavior of the function:
             * 
             * ```
             * Input variable value: 55
             * Shifting amount: 3
             * 
             *          Shifting
             * 00110111 -------> 00000110
             * ```
             * 
             * @param variable Input & Output variable.
             * @param amount Amount of bits to shift.
            **/
            void shift_right(Variable& variable, uint64_t amount);

            /**
             * @brief Shift variable wires to the right. Output does not override initial variable.
             * 
             * -----
             * 
             * This function shifts the variable wires towards the right. This means that each wire is decreasing its significance
             * by X, being X the amount of bits to shift.
             * The next wires that appear on the left are automatically assigned to have a wire value of 0 (zero_wire of the
             * circuit).
             * 
             * The following example shows the expected behavior of the function:
             * 
             * ```
             * Input variable value: 55
             * Shifting amount: 3
             * 
             *          Shifting
             * 00110111 -------> 00000110
             * ```
             * 
             * @param variable Input & Output variable.
             * @param amount Amount of bits to shift.
             * @param output Shifted output.
            **/
            void shift_right(const Variable& variable, uint64_t amount, Variable& output);

            /**
             * @brief Rotates variable wires to the left. Output overrides the input variable.
             * 
             * -----
             * 
             * This function rotates the variable wires towards the left. This is similar to the shift left method except that the
             * most left wires will be placed back on the right of the variable. This means that, for each rotated bit, every wire
             * is increasing its significance except the most significant one, which is rotated back to the right, being the least
             * significat wire now.
             * 
             * The following example shows the expected behavior of the function:
             * 
             * ```
             * Input variable value: 205
             * Rotation amount: 3
             * 
             *          Shifting
             * 11001101 -------> 01101110
             * ```
             * 
             * @param variable Input & Output variable.
             * @param amount Amount of bits to rotate.
            **/
            void rotate_left(Variable& variable, uint64_t amount);

            /**
             * @brief Rotates variable wires to the left. Output does not override initial variable.
             * 
             * -----
             * 
             * This function rotates the variable wires towards the left. This is similar to the shift left method except that the
             * most left wires will be placed back on the right of the variable. This means that, for each rotated bit, every wire
             * is increasing its significance except the most significant one, which is rotated back to the right, being the least
             * significat wire now.
             * 
             * The following example shows the expected behavior of the function:
             * 
             * ```
             * Input variable value: 205
             * Rotation amount: 3
             * 
             *          Shifting
             * 11001101 -------> 01101110
             * ```
             * 
             * @param variable Input & Output variable.
             * @param amount Amount of bits to rotate.
             * @param output Rotated output.
            **/
            void rotate_left(const Variable& variable, uint64_t amount, Variable& output);

            /**
             * @brief Rotates variable wires to the right. Output overrides the input variable.
             * 
             * -----
             * 
             * This function rotates the variable wires towards the right. This is similar to the shift right method except that
             * the most right wires will be placed back on the left of the variable. This means that, for each rotated bit, every
             * wire is decreasing its significance except the least significant one, which is rotated back to the left, being the
             * most significat wire now.
             * 
             * The following example shows the expected behavior of the function:
             * 
             * ```
             * Input variable value: 205
             * Rotation amount: 3
             * 
             *          Shifting
             * 11001101 -------> 10111001
             * ```
             * 
             * @param variable Input & Output variable.
             * @param amount Amount of bits to rotate.
            **/
            void rotate_right(Variable& variable, uint64_t amount);

            /**
             * @brief Rotates variable wires to the right. Output does not override initial variable.
             * 
             * -----
             * 
             * This function rotates the variable wires towards the right. This is similar to the shift right method except that
             * the most right wires will be placed back on the left of the variable. This means that, for each rotated bit, every
             * wire is decreasing its significance except the least significant one, which is rotated back to the left, being the
             * most significat wire now.
             * 
             * The following example shows the expected behavior of the function:
             * 
             * ```
             * Input variable value: 205
             * Rotation amount: 3
             * 
             *          Shifting
             * 11001101 -------> 10111001
             * ```
             * 
             * @param variable Input & Output variable.
             * @param amount Amount of bits to rotate.
             * @param output Rotated output.
            **/
            void rotate_right(const Variable& variable, uint64_t amount, Variable& output);

            /**
             * @brief Flips variable wires. Output overrides the input variable.
             * 
             * -----
             * 
             * This function flips the variable wires. The flipping behavior can be compared to a mirror, i.e., the most left
             * wires will be placed as the most right wires and vice-versa.
             * 
             * The following example shows the expected behavior of the function:
             * 
             * ```
             * Input variable value: 13
             * Rotation amount: 3
             * 
             *          Shifting
             * 00001101 -------> 10110000
             * ```
             * 
             * @param variable Input & Output variable.
            **/
            void flip(Variable& variable);

            /**
             * @brief Flips variable wires. Output does not override initial variable.
             * 
             * -----
             * 
             * This function flips the variable wires. The flipping behavior can be compared to a mirror, i.e., the most left
             * wires will be placed as the most right wires and vice-versa.
             * 
             * The following example shows the expected behavior of the function:
             * 
             * ```
             * Input variable value: 13
             * Rotation amount: 3
             * 
             *          Shifting
             * 00001101 -------> 10110000
             * ```
             * 
             * @param variable Input & Output variable.
             * @param output Flipped output.
            **/
            void flip(const Variable& variable, Variable& output);

            /**
             * @brief Performs the 2's complement to a variable. Output overrides the input variable.
             * 
             * -----
             * 
             * This function performs the 2's complement to the input variable. This operation is used to changed the sign of the
             * variable, i.e, change -X to X and vice-versa. To perform this operation, all the wires of the variable have to be
             * negated (1's complement), and then add the varaible 1 to that result.
             * 
             * The following example shows the expected behavior of the function:
             * 
             * ```
             * Input variable value: 13
             * Output variable value: -13
             * 
             *          2's comp
             * 00001101 -------> 11110011
             * ```
             * 
             * @param variable Input & Output variable.
            **/
            void twos_complement(Variable& variable);

            /**
             * @brief Performs the 2's complement to a variable. Output does not override initial variable.
             * 
             * -----
             * 
             * This function performs the 2's complement to the input variable. This operation is used to changed the sign of the
             * variable, i.e, change -X to X and vice-versa. To perform this operation, all the wires of the variable have to be
             * negated (1's complement), and then add the varaible 1 to that result.
             * 
             * The following example shows the expected behavior of the function:
             * 
             * ```
             * Input variable value: 13
             * Output variable value: -13
             * 
             *          2's comp
             * 00001101 -------> 11110011
             * ```
             * 
             * @param variable Input & Output variable.
             * @param output 2's complement output.
            **/
            void twos_complement(const Variable& variable, Variable& output);

        // Basic operations
        public:
            /**
             * @brief XOR operation between two wires (bits).
             * 
             * -----
             * 
             * An XOR gate is represented by following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$XOR\f$ |
             * | :-----: | :-----: | :-------: |
             * | 0       | 0       | 0         |
             * | 0       | 0       | 1         |
             * | 0       | 1       | 1         |
             * | 0       | 1       | 0         |
             * 
             * @param in_a Input wire \f$A\f$.
             * @param in_b Input wire \f$B\f$.
             * @param out Output wire.
            **/
            void XOR(const Wire in_a, const Wire in_b, Wire& out);
            
            /**
             * @brief XOR operation between two variables.
             * 
             * -----
             * 
             * An XOR gate is represented by following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$XOR\f$ |
             * | :-----: | :-----: | :-------: |
             * | 0       | 0       | 0         |
             * | 0       | 0       | 1         |
             * | 0       | 1       | 1         |
             * | 0       | 1       | 0         |
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable.
            **/
            void XOR(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief AND operation between two wires (bits).
             * 
             * -----
             * 
             * An AND gate is represented by following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$AND\f$ |
             * | :-----: | :-----: | :-------: |
             * | 0       | 0       | 0         |
             * | 0       | 0       | 0         |
             * | 0       | 1       | 0         |
             * | 0       | 1       | 1         |
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable.
            **/
            void AND(const Wire in_a, const Wire in_b, Wire& out);

            /**
             * @brief AND operation between two variables.
             * 
             * -----
             * 
             * An AND gate is represented by following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$AND\f$ |
             * | :-----: | :-----: | :-------: |
             * | 0       | 0       | 0         |
             * | 0       | 0       | 0         |
             * | 0       | 1       | 0         |
             * | 0       | 1       | 1         |
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable.
            **/
            void AND(const Variable& in_a, const Variable& in_b, Variable& out);
            
            /**
             * @brief OR operation between two wires (bits).
             * 
             * -----
             * 
             * An OR gate is represented by following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$OR\f$ |
             * | :-----: | :-----: | :------: |
             * | 0       | 0       | 0        |
             * | 0       | 0       | 1        |
             * | 0       | 1       | 1        |
             * | 0       | 1       | 1        |
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable.
            **/
            void OR(const Wire in_a, const Wire in_b, Wire& out);

            /**
             * @brief OR operation between two variables.
             * 
             * -----
             * 
             * An OR gate is represented by following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$OR\f$ |
             * | :-----: | :-----: | :------: |
             * | 0       | 0       | 0        |
             * | 0       | 0       | 1        |
             * | 0       | 1       | 1        |
             * | 0       | 1       | 1        |
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable.
            **/
            void OR(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief INV operation between two wires (bits).
             * 
             * -----
             * 
             * An INV gate is represented by following truth table:
             * 
             * | \f$A\f$ | \f$INV\f$ |
             * | :-----: | :-------: |
             * | 0       | 1         |
             * | 1       | 0         |
             * 
             * @param in_a Input variable \f$A\f$.
             * @param out Output variable.
            **/
            void INV(const Wire in_a, Wire& out);

            /**
             * @brief INV operation between two variables.
             * 
             * -----
             * 
             * An INV gate is represented by following truth table:
             * 
             * | \f$A\f$ | \f$INV\f$ |
             * | :-----: | :-------: |
             * | 0       | 1         |
             * | 1       | 0         |
             * 
             * @param in_a Input variable \f$A\f$.
             * @param out Output variable.
            **/
            void INV(const Variable& in_a, Variable& out);

            /**
             * @brief XNOR operation between two wires (bits).
             * 
             * -----
             * 
             * An XNOR operation is represented by following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$XNOR\f$ |
             * | :-----: | :-----: | :--------: |
             * | 0       | 0       | 1          |
             * | 0       | 0       | 0          |
             * | 0       | 1       | 0          |
             * | 0       | 1       | 1          |
             * 
             * However, the XNOR is not a basic logic gate. It is, in fact, an operation that makes use of two basic logic gate,
             * i.e an XOR gate followed by an INV gate.
             * This method exists just to agile the need to use these two operations in a single method call.
             * 
             * @param in_a Input wire \f$A\f$.
             * @param in_b Input wire \f$B\f$.
             * @param out Output wire.
            **/
            void XNOR(const Wire in_a, const Wire in_b, Wire& out);
            
            /**
             * @brief XNOR operation between two variables.
             * 
             * -----
             * 
             * An XNOR operation is represented by following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$XNOR\f$ |
             * | :-----: | :-----: | :--------: |
             * | 0       | 0       | 1          |
             * | 0       | 0       | 0          |
             * | 0       | 1       | 0          |
             * | 0       | 1       | 1          |
             *
             * However, the XNOR is not a basic logic gate. It is, in fact, an operation that makes use of two basic logic gate,
             * i.e an XOR gate followed by an INV gate.
             * This method exists just to agile the need to use these two operations in a single method call.
             *
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable.
            **/
            void XNOR(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief NAND operation between two wires (bits).
             * 
             * -----
             * 
             * An NAND operation is represented by following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$XNOR\f$ |
             * | :-----: | :-----: | :--------: |
             * | 0       | 0       | 1          |
             * | 0       | 0       | 1          |
             * | 0       | 1       | 1          |
             * | 0       | 1       | 0          |
             * 
             * However, the NAND is not a basic logic gate. It is, in fact, an operation that makes use of two basic logic gate,
             * i.e an AND gate followed by an INV gate.
             * This method exists just to agile the need to use these two operations in a single method call.
             * 
             * @param in_a Input wire \f$A\f$.
             * @param in_b Input wire \f$B\f$.
             * @param out Output wire.
            **/
            void NAND(const Wire in_a, const Wire in_b, Wire& out);
            
            /**
             * @brief NAND operation between two wires (bits).
             * 
             * -----
             * 
             * An NAND operation is represented by following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$XNOR\f$ |
             * | :-----: | :-----: | :--------: |
             * | 0       | 0       | 1          |
             * | 0       | 0       | 1          |
             * | 0       | 1       | 1          |
             * | 0       | 1       | 0          |
             * 
             * However, the NAND is not a basic logic gate. It is, in fact, an operation that makes use of two basic logic gate,
             * i.e an AND gate followed by an INV gate.
             * This method exists just to agile the need to use these two operations in a single method call.
             * 
             * @param in_a Input wire \f$A\f$.
             * @param in_b Input wire \f$B\f$.
             * @param out Output wire.
            **/
            void NAND(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief NOR operation between two wires (bits).
             * 
             * -----
             * 
             * An NOR operation is represented by following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$XNOR\f$ |
             * | :-----: | :-----: | :--------: |
             * | 0       | 0       | 1          |
             * | 0       | 0       | 0          |
             * | 0       | 1       | 0          |
             * | 0       | 1       | 0          |
             * 
             * However, the NOR is not a basic logic gate. It is, in fact, an operation that makes use of two basic logic gate,
             * i.e an OR gate followed by an INV gate.
             * This method exists just to agile the need to use these two operations in a single method call.
             * 
             * @param in_a Input wire \f$A\f$.
             * @param in_b Input wire \f$B\f$.
             * @param out Output wire.
            **/
            void NOR(const Wire in_a, const Wire in_b, Wire& out);
            
            /**
             * @brief NOR operation between two wires (bits).
             * 
             * -----
             * 
             * An NOR operation is represented by following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$XNOR\f$ |
             * | :-----: | :-----: | :--------: |
             * | 0       | 0       | 1          |
             * | 0       | 0       | 0          |
             * | 0       | 1       | 0          |
             * | 0       | 1       | 0          |
             * 
             * However, the NOR is not a basic logic gate. It is, in fact, an operation that makes use of two basic logic gate,
             * i.e an OR gate followed by an INV gate.
             * This method exists just to agile the need to use these two operations in a single method call.
             * 
             * @param in_a Input wire \f$A\f$.
             * @param in_b Input wire \f$B\f$.
             * @param out Output wire.
            **/
            void NOR(const Variable& in_a, const Variable& in_b, Variable& out);

        // Arithmetic operations
        public:
            /**
             * @brief Binary addition between two variables, resulting in a new variable.
             * 
             * -----
             * 
             * @details Addition is one of the most basic operations performed and it is referred as a binary adder in logic
             * circuits. There are two adder types: half adder, and full adder. This function is developed to target a binary
             * addition that uses a full adder. A full adder is a combinational logic circuit that adds two binary input bits
             * (\f$A\f$ and \f$B\f$), and a carry bit (\f$C_{in}\f$), resulting in a sum bit (\f$Sum\f$) and another carry bit
             * (\f$C_{out}\f$). \f$C_{in}\f$ and \f$C_{out}\f$ are just the carry bits from the previous and to the next
             * operation, respectively.
             * 
             * A logical adder is variable sign independent, which means it works the same way for signed and unsigned variables.
             * 
             * A full adder can be represented has the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$C_{in}\f$ | \f$Sum\f$ | \f$C_{out}\f$ |
             * | :-----: | :-----: | :----------: | :-------: | :-----------: |
             * | 0       | 0       | 0            | 0         | 0             |
             * | 0       | 0       | 1            | 1         | 0             |
             * | 0       | 1       | 0            | 1         | 0             |
             * | 0       | 1       | 1            | 0         | 1             |
             * | 1       | 0       | 0            | 1         | 0             |
             * | 1       | 0       | 1            | 0         | 1             |
             * | 1       | 1       | 0            | 0         | 1             |
             * | 1       | 1       | 1            | 1         | 1             |
             * 
             * The truth table translates a logical expression for both \f$Sum\f$ and \f$C_{out}\f$ outputs. These expressions can
             * be further simplified, as follows:
             * 
             * \begin{align*}
             *      Sum &= \overline{A}.\overline{B}.C_{in} + \overline{A}.B.\overline{C_{in}} + A.\overline{B}.\overline{C_{in}} + A.B.C_{in}\\\
             *          &= C_{in}.(\overline{A}.\overline{B} + A.B) + \overline{C_{in}}.(\overline{A}.B + A.\overline{B})\\\
             *          &= C_{in}.(\overline{A \oplus B}) + \overline{C_{in}}.(A \oplus B)\\\
             *          &= C_{in} \oplus (A \oplus B)
             * \end{align*}
             *
             * \begin{align*}
             *      C_{out} &= \overline{A}.B.C_{in} + A.\overline{B}.C_{in} + A.B.\overline{C_{in}} + A.B.C_{in}\\\
             *              &= C_{in}.(\overline{A}.B + A.\overline{B}) + A.B\\\
             *              &= C_{in}.(A \oplus B) + A.B
             * \end{align*}
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable \f$Sum\f$.
            **/
            void sum(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief Binary subtraction between two variables, resulting in a new variable.
             * 
             * -----
             * 
             * @details Subtraction is one of the most basic operations performed and it is referred as a binary subtractor in
             * logic circuits. There are two subtractor types: half subtractor, and full subtractor. This function is developed to
             * target a binary subtraction that uses a full subtractor. A full subtractor is a combinational logic circuit that
             * makes the difference between two binary input bits (\f$A\f$, minuend, and \f$B\f$, subtrahend), taken into account
             * the borrow bit (\f$B_{in}\f$), resulting in a sub bit (\f$Sub\f$) and another borrow bit (\f$B_{out}\f$).
             * \f$B_{in}\f$ and \f$B_{out}\f$ are just the borrow bits from the previous and to the next operation, respectively.
             * 
             * A logical subtractor is variable sign independent, which means it works the same way for signed and unsigned
             * variables.
             * 
             * A full subtractor can be represented has the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$B_{in}\f$ | \f$Sub\f$ | \f$B_{out}\f$ |
             * | :-----: | :-----: | :----------: | :-------: | :-----------: |
             * | 0       | 0       | 0            | 0         | 0             |
             * | 0       | 0       | 1            | 1         | 1             |
             * | 0       | 1       | 0            | 1         | 1             |
             * | 0       | 1       | 1            | 0         | 1             |
             * | 1       | 0       | 0            | 1         | 0             |
             * | 1       | 0       | 1            | 0         | 0             |
             * | 1       | 1       | 0            | 0         | 0             |
             * | 1       | 1       | 1            | 1         | 1             |
             * 
             * The truth table translates a logical expression for both \f$Sub\f$ and \f$B_{out}\f$ outputs. These expressions can
             * be further simplified, as follows:
             * 
             * \begin{align*}
             *      Sub &= \overline{A}.\overline{B}.B_{in} + \overline{A}.B.\overline{B_{in}} + A.\overline{B}.\overline{B_{in}} + A.B.B_{in}\\\
             *          &= B_{in}.(\overline{A}.\overline{B} + A.B) + \overline{B_{in}}.(\overline{A}.B + A.\overline{B})\\\
             *          &= B_{in}.(\overline{A \oplus B}) + \overline{B_{in}}.(A \oplus B)\\\
             *          &= B_{in} \oplus (A \oplus B)
             * \end{align*}
             *
             * \begin{align*}
             *      B_{out} &= \overline{A}.\overline{B}.B_{in} + \overline{A}.B.\overline{B_{in}} + \overline{A}.B.B_{in} + A.B.B_{in}\\\
             *              &= \overline{A}.(\overline{B}.B_{in} + B.\overline{B_{in}}) + B.B_{in}\\\
             *              &= \overline{A}.(B \oplus B_{in}) + B.B_{in}
             * \end{align*}
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable \f$Sub\f$.
            **/
            void subtract(const Variable& in_a, const Variable& in_b, Variable& out);
            
            /**
             * @brief Binary multiplication between two unsigned variables, resulting in a new unsigned variable.
             * 
             * -----
             * 
             * @details Binary multiplication, also known as array multiplier, is a digital combinational circuit used for
             * multiplying two binary numbers.
             * The circuit creation for a binary multiplication is dependent on the sign of the input variables. As such, this
             * function is directed to the multiplication between two unsigned variables, thus the "u" in the function name.
             * An unsigned binary multiplication operation can be described by several addition operations by emplying an array
             * fill of full adders and half adders.
             * 
             * For better understanding, take a look at the following example: 
             * 
             * ```
             *       0111 (7)   <-- Input variable A
             *     x 0111 (7)   <-- Input variable B
             *  ------------- 
             *       0111       <-- AND (A0, B)
             *    + 0111        <-- AND (A1, B)
             *   + 0111         <-- AND (A2, B)
             *  + 0000          <-- AND (A3, B)
             *  ------------- 
             *   00110001 (49)  <-- Multiplication result
             * ```
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable \f$Mult\f$.
            **/
            void multiply_u(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief Binary multiplication between two signed variables, resulting in a new signed variable.
             * 
             * -----
             * 
             * @details Binary multiplication, also known as array multiplier, is a digital combinational circuit used for
             * multiplying two binary numbers.
             * The circuit creation for a binary multiplication is dependent on the sign of the input variables. As such, this
             * function is directed to the multiplication between two signed variables, thus the "s" in the function name.
             * A signed binary multiplication operation can be described by several addition operations by emplying an array fill
             * of full adders and half adders, followed by a single subtraction operation.
             *
             * For better understanding, take a look at the following example: 
             * 
             * ```
             *      0111 (7)   <-- Input variable A
             *    x 1001 (-7)  <-- Input variable B
             * ------------- 
             *   0000111       <-- AND (A0, B)
             * + 000000        <-- AND (A1, B)
             * + 00000         <-- AND (A2, B)
             * - 0111          <-- AND (A3, B)
             * ------------- 
             *  11001111 (-49) <-- Multiplication result
             * ```
             * 
             * Alternatively, the binary multiplication of signed variables can also be described by a series of only addition
             * operations, if a tweak is made.
             * 
             * ```
             *       0111 (7)   <-- Input variable A
             *     x 1001 (-7)  <-- Input variable B
             * --------------- 
             *       1111       <-- AND (A0, B) (most significant bit is inverted)
             *    + 1000        <-- AND (A1, B) (most significant bit is inverted)
             *   + 1000         <-- AND (A2, B) (most significant bit is inverted)
             *  + 0000          <-- AND (A3, B) (all bits inverted except the most significant bit)
             * + 1001          
             * --------------- 
             *   11001111 (-49) <-- Multiplication result
             * ```
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable \f$Mult\f$.
            **/
            void multiply_s(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief Binary division between two unsigned variables, resulting in a two new unsigned variables (quotient and
             * remainder).
             * 
             * -----
             * 
             * @details Binary division is a digital combinational circuit used to divide two binary numbers. In a division, two
             * outputs are expected, the quotient (division result), and the remainder (what cannot be divided and remains).
             * The circuit creation for a binary division is dependent on the sign of the input variables. As such, this function
             * is directed to the division between two unsigned variables, thus the "u" in the function name.
             * An unsigned binary division operation can be represented by the cycle of successive compare, shift, and subtract
             * operations.
             * 
             * For better understanding, take a look at the following example: 
             * 
             * ```
             *  Dividend | Divisor
             *    (In A) | (In B)
             *      1101 | 0010
             *  ------------------ 
             *  0000     |         <-- Initial remainder value.
             *   0001    |         <-- Shifting. Assign dividend bit.
             * - 0000    | 0       <-- Remainder < Divisor.
             *  ------------------ 
             *   0001    | 0       <-- Subtraction.
             *    0011   | 0       <-- Shifting. Assign dividend bit.
             *  - 0010   | 01      <-- Remainder >= Divisor.
             *  ------------------ 
             *    0001   | 01      <-- Subtraction.
             *     0010  | 01      <-- Shifting. Assign dividend bit.
             *   - 0010  | 011     <-- Remainder >= Divisor.
             *  ------------------ 
             *     0000  | 011     <-- Subtraction.
             *      0001 | 011     <-- Shifting. Assign dividend bit.
             *    - 0000 | 0110    <-- Remainder < Divisor.
             *  ------------------ 
             *      0001 | 0110    <-- Subtraction.
             *   (Out R) | (Out Q)
             * Remainder | Quotient
             * ```
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out_q Output variable \f$Q\f$ (Quotient).
             * @param out_r Output variable \f$R\f$ (Remainder).
            **/
            void divide_u(const Variable& in_a, const Variable& in_b, Variable& out_q, Variable& out_r);

            /**
             * @brief Binary division between two unsigned variables, resulting in a new unsigned variable (quotient).
             * 
             * -----
             * 
             * @details Binary division is a digital combinational circuit used to divide two binary numbers. In a division, two
             * outputs are expected, the quotient (division result), and the remainder (what cannot be divided and remains).
             * However, this function is designed to only return the quotient value, ignoring the remainder.
             * The circuit creation for a binary division is dependent on the sign of the input variables. As such, this function
             * is directed to the division between two unsigned variables, thus the "u" in the function name.
             * An unsigned binary division operation can be represented by the cycle of successive compare, shift, and subtract
             * operations.
             * 
             * For better understanding, take a look at the following example: 
             * 
             * ```
             *  Dividend | Divisor
             *    (In A) | (In B)
             *      1101 | 0010
             *  ------------------ 
             *  0000     |         <-- Initial remainder value.
             *   0001    |         <-- Shifting. Assign dividend bit.
             * - 0000    | 0       <-- Remainder < Divisor.
             *  ------------------ 
             *   0001    | 0       <-- Subtraction.
             *    0011   | 0       <-- Shifting. Assign dividend bit.
             *  - 0010   | 01      <-- Remainder >= Divisor.
             *  ------------------ 
             *    0001   | 01      <-- Subtraction.
             *     0010  | 01      <-- Shifting. Assign dividend bit.
             *   - 0010  | 011     <-- Remainder >= Divisor.
             *  ------------------ 
             *     0000  | 011     <-- Subtraction.
             *      0001 | 011     <-- Shifting. Assign dividend bit.
             *    - 0000 | 0110    <-- Remainder < Divisor.
             *  ------------------ 
             *      0001 | 0110    <-- Subtraction.
             *   (Out R) | (Out Q)
             * Remainder | Quotient
             * ```
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out_q Output variable \f$Q\f$ (Quotient).
            **/
            void divide_u_quotient(const Variable& in_a, const Variable& in_b, Variable& out_q);

            /**
             * @brief Binary division between two unsigned variables, resulting in a new unsigned variable (remainder).
             * 
             * -----
             * 
             * @details Binary division is a digital combinational circuit used to divide two binary numbers. In a division, two
             * outputs are expected, the quotient (division result), and the remainder (what cannot be divided and remains).
             * However, this function is designed to only return the remainder value, ignoring the quotient.
             * The circuit creation for a binary division is dependent on the sign of the input variables. As such, this function
             * is directed to the division between two unsigned variables, thus the "u" in the function name.
             * An unsigned binary division operation can be represented by the cycle of successive compare, shift, and subtract
             * operations.
             * 
             * For better understanding, take a look at the following example: 
             * 
             * ```
             *  Dividend | Divisor
             *    (In A) | (In B)
             *      1101 | 0010
             *  ------------------ 
             *  0000     |         <-- Initial remainder value.
             *   0001    |         <-- Shifting. Assign dividend bit.
             * - 0000    | 0       <-- Remainder < Divisor.
             *  ------------------ 
             *   0001    | 0       <-- Subtraction.
             *    0011   | 0       <-- Shifting. Assign dividend bit.
             *  - 0010   | 01      <-- Remainder >= Divisor.
             *  ------------------ 
             *    0001   | 01      <-- Subtraction.
             *     0010  | 01      <-- Shifting. Assign dividend bit.
             *   - 0010  | 011     <-- Remainder >= Divisor.
             *  ------------------ 
             *     0000  | 011     <-- Subtraction.
             *      0001 | 011     <-- Shifting. Assign dividend bit.
             *    - 0000 | 0110    <-- Remainder < Divisor.
             *  ------------------ 
             *      0001 | 0110    <-- Subtraction.
             *   (Out R) | (Out Q)
             * Remainder | Quotient
             * ```
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out_r Output variable \f$R\f$ (Remainder).
            **/
            void divide_u_remainder(const Variable& in_a, const Variable& in_b, Variable& out_r);

            /**
             * @brief Binary division between two signed variables, resulting in a two new signed variables (quotient and
             * remainder).
             * 
             * -----
             * 
             * @details Binary division is a digital combinational circuit used to divide two binary numbers. In a division, two
             * outputs are expected, the quotient (division result), and the remainder (what cannot be divided and remains).
             * The circuit creation for a binary division is dependent on the sign of the input variables. As such, this function
             * is directed to the division between two signed variables, thus the "s" in the function name.
             * A signed binary division operation can be represented by the cycle of successive compare, shift, and subtract
             * operations.
             * 
             * A signed binary division is similar to the unsigned binary division, with the addition of a "pre-processing" step
             * to the inputs, where these variables are manipulated acoordingly to their signs, and a "post-processing" step to
             * the quotient output, where the input signs manipulate it.
             * 
             * For better understanding of the variable manipulation, take a look at the following table:
             * 
             * | \f$A\f$ sign | \f$B\f$ sign | Dividend    | Divisor     | Quotient    |
             * | :----------: | :----------: | :---------: | :---------: | :---------: |
             * | 0            | 0            | \f$A\f$     | \f$B\f$     | \f$Q\f$     |
             * | 0            | 1            | \f$A\f$     | 2's \f$B\f$ | 2's \f$Q\f$ |
             * | 1            | 0            | 2's \f$A\f$ | \f$B\f$     | 2's \f$Q\f$ |
             * | 1            | 1            | 2's \f$A\f$ | 2's \f$B\f$ | \f$Q\f$     |
             * 
             * With the dividend and divisor variables defined, they can be used in an unsigned binary division, since they were
             * manipulated to setup the conditions to allow the unsigned operation.
             * Since the output comes from the unsigned binary operation, it also has to be manipulated in case its result is
             * suppose to be negative. This only happens when the operation is between two signed variables with different signs.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out_q Output variable \f$Q\f$ (Quotient).
             * @param out_r Output variable \f$R\f$ (Remainder).
            **/
            void divide_s(const Variable& in_a, const Variable& in_b, Variable& out_q, Variable& out_r);

            /**
             * @brief Binary division between two signed variables, resulting in a new signed variable (quotient).
             * 
             * -----
             * 
             * @details Binary division is a digital combinational circuit used to divide two binary numbers. In a division, two
             * outputs are expected, the quotient (division result), and the remainder (what cannot be divided and remains).
             * The circuit creation for a binary division is dependent on the sign of the input variables. As such, this function
             * is directed to the division between two signed variables, thus the "s" in the function name.
             * A signed binary division operation can be represented by the cycle of successive compare, shift, and subtract
             * operations.
             * 
             * A signed binary division is similar to the unsigned binary division, with the addition of a "pre-processing" step
             * to the inputs, where these variables are manipulated acoordingly to their signs, and a "post-processing" step to
             * the quotient output, where the input signs manipulate it.
             * 
             * For better understanding of the variable manipulation, take a look at the following table:
             * 
             * | \f$A\f$ sign | \f$B\f$ sign | Dividend    | Divisor     | Quotient    |
             * | :----------: | :----------: | :---------: | :---------: | :---------: |
             * | 0            | 0            | \f$A\f$     | \f$B\f$     | \f$Q\f$     |
             * | 0            | 1            | \f$A\f$     | 2's \f$B\f$ | 2's \f$Q\f$ |
             * | 1            | 0            | 2's \f$A\f$ | \f$B\f$     | 2's \f$Q\f$ |
             * | 1            | 1            | 2's \f$A\f$ | 2's \f$B\f$ | \f$Q\f$     |
             * 
             * With the dividend and divisor variables defined, they can be used in an unsigned binary division, since they were
             * manipulated to setup the conditions to allow the unsigned operation.
             * Since the output comes from the unsigned binary operation, it also has to be manipulated in case its result is
             * suppose to be negative. This only happens when the operation is between two signed variables with different signs.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out_q Output variable \f$Q\f$ (Quotient).
            **/
            void divide_s_quotient(const Variable& in_a, const Variable& in_b, Variable& out_q);

            /**
             * @brief Binary division between two signed variables, resulting in a new signed variable (remainder).
             * 
             * -----
             * 
             * @details Binary division is a digital combinational circuit used to divide two binary numbers. In a division, two
             * outputs are expected, the quotient (division result), and the remainder (what cannot be divided and remains).
             * The circuit creation for a binary division is dependent on the sign of the input variables. As such, this function
             * is directed to the division between two signed variables, thus the "s" in the function name.
             * A signed binary division operation can be represented by the cycle of successive compare, shift, and subtract
             * operations.
             * 
             * For better understanding of the variable manipulation, take a look at the following table:
             * 
             * | \f$A\f$ sign | \f$B\f$ sign | Dividend    | Divisor     | Quotient    |
             * | :----------: | :----------: | :---------: | :---------: | :---------: |
             * | 0            | 0            | \f$A\f$     | \f$B\f$     | \f$Q\f$     |
             * | 0            | 1            | \f$A\f$     | 2's \f$B\f$ | 2's \f$Q\f$ |
             * | 1            | 0            | 2's \f$A\f$ | \f$B\f$     | 2's \f$Q\f$ |
             * | 1            | 1            | 2's \f$A\f$ | 2's \f$B\f$ | \f$Q\f$     |
             * 
             * With the dividend and divisor variables defined, they can be used in an unsigned binary division, since they were
             * manipulated to setup the conditions to allow the unsigned operation.
             * Since this function only targets to get the remainder result, the very last step of manipulating the quotient
             * variable is not needed.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out_r Output variable \f$R\f$ (Remainder).
            **/
            void divide_s_remainder(const Variable& in_a, const Variable& in_b, Variable& out_r);
        
        // Conditional operations
        public:
            /**
             * @brief Output \f$A\f$ or \f$B\f$ depending on the value of a control bit \f$C\f$.
             * 
             * -----
             * 
             * A multiplexer is a combinational circuit which can have several inputs but only a single output. This output is one
             * of the inputs, and its selection is decided by control(s)/selection(s) inputs.
             * 
             * A multiplexer can be designed to have as many inputs as pleased, as long as there are enough control/selection bits
             * combinations. In this case, this method was designed to be a 2x1 multiplexer, which means that it uses a single
             * control bit to manage which of the two inputs to output.
             * 
             * This method has the following truth table:
             * 
             * | \f$C\f$ | \f$Decision\f$ |
             * | :-----: | :------------: |
             * | 0       | \f$A\f$        |
             * | 1       | \f$B\f$        |
             * 
             * From the truth table we can achieve the following expression:
             * 
             * \begin{align*}
             *      Decision &= \overline{C}.A + C.B
             * \end{align*}
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param in_c Input control bit \f$C\f$.
             * @param out Output variable \f$Decision\f$.
            **/
            void multiplexer(const Variable& in_a, const Variable& in_b, const Wire& in_c, Variable& out);

            /**
             * @brief Evaluates if two variables are equal or not.
             * 
             * -----
             * 
             * The equality between two bits is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$=\f$ |
             * | :-----: | :-----: | :-----: |
             * | 0       | 0       | 1       |
             * | 0       | 1       | 0       |
             * | 1       | 0       | 0       |
             * | 1       | 1       | 1       |
             * 
             * As can be seen, it represents an XNOR gate.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output wire \f$Equal\f$.
            **/
            void equal(const Variable& in_a, const Variable& in_b, Wire& out);

            /**
             * @brief Evaluates if two variables are equal or not.
             * 
             * -----
             * 
             * The equality between two bits is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$=\f$ |
             * | :-----: | :-----: | :-----: |
             * | 0       | 0       | 1       |
             * | 0       | 1       | 0       |
             * | 1       | 0       | 0       |
             * | 1       | 1       | 1       |
             * 
             * As can be seen, it represents an XNOR gate.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable \f$Equal\f$.
            **/
            void equal(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief Evaluates if unsigned variable A is greater than unsigned variable B.
             * 
             * -----
             * 
             * The circuit creation for a greater operation is dependent on the sign of the input variables. As such, this
             * function is directed to an operation between two unsigned variables, thus the "u" in the function name.
             * 
             * The greater binary operation for unsigned variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$>\f$ |
             * | :-----: | :-----: | :-----: |
             * | 0       | 0       | 0       |
             * | 0       | 1       | 0       |
             * | 1       | 0       | 1       |
             * | 1       | 1       | 0       |
             * 
             * From the truth table, we can conclude that A is only bigger than B if: \f$A.\overline{B}\f$
             * 
             * However, a greater binary operation has different weights for all the bits in a variable, i.e, the more significant
             * the bit is, the more weight it has in the operation.
             * For example, in hypothetical terms, we are comparing 2 variables (A and B) both with 3 bits, A is greater than B if:
             * ( A2 > B2 ) OR ( A2 = B2 AND A1 > B1 ) OR (A2 = B2 AND A1 = B1 AND A0 > B0 )
             * 
             * This can be translated into the following generic relation:
             * 
             * \begin{align*}
             *      A>B &= A_{n}.\overline{B_{n}} \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(A_{n-1}.\overline{B_{n-1}}) \\\
             *          &+ (...) \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(\overline{A_{n-1} \oplus B_{n-1}}).(...).(\overline{A_{1} \oplus B_{1}}).(A_{0}.\overline{B_{0}})
             * \end{align*}
             * 
             * Where \f$n\f$ is the number of bits.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output wire \f$Greater\f$.
            **/
            void greater_u(const Variable& in_a, const Variable& in_b, Wire& out);

            /**
             * @brief Evaluates if unsigned variable A is greater than unsigned variable B.
             * 
             * -----
             * 
             * The circuit creation for a greater operation is dependent on the sign of the input variables. As such, this
             * function is directed to an operation between two unsigned variables, thus the "u" in the function name.
             * 
             * The greater binary operation for unsigned variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$>\f$ |
             * | :-----: | :-----: | :-----: |
             * | 0       | 0       | 0       |
             * | 0       | 1       | 0       |
             * | 1       | 0       | 1       |
             * | 1       | 1       | 0       |
             * 
             * From the truth table, we can conclude that A is only bigger than B if: \f$A.\overline{B}\f$
             * 
             * However, a greater binary operation has different weights for all the bits in a variable, i.e, the more significant
             * the bit is, the more weight it has in the operation.
             * For example, in hypothetical terms, we are comparing 2 variables (A and B) both with 3 bits, A is greater than B if:
             * ( A2 > B2 ) OR ( A2 = B2 AND A1 > B1 ) OR (A2 = B2 AND A1 = B1 AND A0 > B0 )
             * 
             * This can be translated into the following generic relation:
             * 
             * \begin{align*}
             *      A>B &= A_{n}.\overline{B_{n}} \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(A_{n-1}.\overline{B_{n-1}}) \\\
             *          &+ (...) \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(\overline{A_{n-1} \oplus B_{n-1}}).(...).(\overline{A_{1} \oplus B_{1}}).(A_{0}.\overline{B_{0}})
             * \end{align*}
             * 
             * Where \f$n\f$ is the number of bits.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable \f$Greater\f$.
            **/
            void greater_u(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief Evaluates if signed variable A is greater than signed variable B.
             * 
             * -----
             * 
             * The circuit creation for a greater operation is dependent on the sign of the input variables. As such, this
             * function is directed to an operation between two signed variables, thus the "s" in the function name.
             * 
             * The greater binary operation for signed variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$>\f$ |
             * | :-----: | :-----: | :-----: |
             * | 0       | 0       | 0       |
             * | 0       | 1       | 1       |
             * | 1       | 0       | 0       |
             * | 1       | 1       | 0       |
             * 
             * From the truth table, we can conclude that A is only bigger than B if: \f$\overline{A}.B\f$
             * 
             * However, a greater binary operation has different weights for all the bits in a variable, i.e, the more significant
             * the bit is, the more weight it has in the operation.
             * For example, in hypothetical terms, we are comparing 2 variables (A and B) both with 3 bits, A is greater than B if:
             * ( A2 < B2 ) OR ( A2 = B2 AND A1 > B1 ) OR (A2 = B2 AND A1 = B1 AND A0 > B0 )
             * 
             * This can be translated into the following generic relation:
             * 
             * \begin{align*}
             *      A>B &= \overline{A_{n}}.B_{n} \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(A_{n-1}.\overline{B_{n-1}}) \\\
             *          &+ (...) \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(\overline{A_{n-1} \oplus B_{n-1}}).(...).(\overline{A_{1} \oplus B_{1}}).(A_{0}.\overline{B_{0}})
             * \end{align*}
             * 
             * Where \f$n\f$ is the number of bits.
             * 
             * If we take a look at a greater unsigned operation, the signed operation has exactly the same expression, expect the
             * comparison between the most significant bits, which represent the sign.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output wire \f$Greater\f$.
            **/
            void greater_s(const Variable& in_a, const Variable& in_b, Wire& out);

            /**
             * @brief Evaluates if signed variable A is greater than signed variable B.
             * 
             * -----
             * 
             * The circuit creation for a greater operation is dependent on the sign of the input variables. As such, this
             * function is directed to an operation between two signed variables, thus the "s" in the function name.
             * 
             * The greater binary operation for signed variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$>\f$ |
             * | :-----: | :-----: | :-----: |
             * | 0       | 0       | 0       |
             * | 0       | 1       | 1       |
             * | 1       | 0       | 0       |
             * | 1       | 1       | 0       |
             * 
             * From the truth table, we can conclude that A is only bigger than B if: \f$\overline{A}.B\f$
             * 
             * However, a greater binary operation has different weights for all the bits in a variable, i.e, the more significant
             * the bit is, the more weight it has in the operation.
             * For example, in hypothetical terms, we are comparing 2 variables (A and B) both with 3 bits, A is greater than B if:
             * ( A2 < B2 ) OR ( A2 = B2 AND A1 > B1 ) OR (A2 = B2 AND A1 = B1 AND A0 > B0 )
             * 
             * This can be translated into the following generic relation:
             * 
             * \begin{align*}
             *      A>B &= \overline{A_{n}}.B_{n} \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(A_{n-1}.\overline{B_{n-1}}) \\\
             *          &+ (...) \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(\overline{A_{n-1} \oplus B_{n-1}}).(...).(\overline{A_{1} \oplus B_{1}}).(A_{0}.\overline{B_{0}})
             * \end{align*}
             * 
             * Where \f$n\f$ is the number of bits.
             * 
             * If we take a look at a greater unsigned operation, the signed operation has exactly the same expression, expect the
             * comparison between the most significant bits, which represent the sign.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable \f$Greater\f$.
            **/
            void greater_s(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief Evaluates if unsigned variable A is smaller than unsigned variable B.
             * 
             * -----
             * 
             * The circuit creation for a smaller operation is dependent on the sign of the input variables. As such, this
             * function is directed to an operation between two unsigned variables, thus the "u" in the function name.
             * 
             * The smaller binary operation for unsigned variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$<\f$ |
             * | :-----: | :-----: | :-----: |
             * | 0       | 0       | 0       |
             * | 0       | 1       | 1       |
             * | 1       | 0       | 0       |
             * | 1       | 1       | 0       |
             * 
             * From the truth table, we can conclude that A is only smaller than B if: \f$\overline{A}.B\f$
             * 
             * However, a smaller binary operation has different weights for all the bits in a variable, i.e, the more significant
             * the bit is, the more weight it has in the operation.
             * For example, in hypothetical terms, we are comparing 2 variables (A and B) both with 3 bits, A is smaller than B if:
             * ( A2 < B2 ) OR ( A2 = B2 AND A1 < B1 ) OR (A2 = B2 AND A1 = B1 AND A0 < B0 )
             * 
             * This can be translated into the following generic relation:
             * 
             * \begin{align*}
             *      A<B &= \overline{A_{n}}.B_{n} \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(\overline{A_{n-1}}.B_{n-1}) \\\
             *          &+ (...) \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(\overline{A_{n-1} \oplus B_{n-1}}).(...).(\overline{A_{1} \oplus B_{1}}).(\overline{A_{0}}.B_{0})
             * \end{align*}
             * 
             * Where \f$n\f$ is the number of bits.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output wire \f$Smaller\f$.
            **/
            void smaller_u(const Variable& in_a, const Variable& in_b, Wire& out);

            /**
             * @brief Evaluates if unsigned variable A is smaller than unsigned variable B.
             * 
             * -----
             * 
             * The circuit creation for a smaller operation is dependent on the sign of the input variables. As such, this
             * function is directed to an operation between two unsigned variables, thus the "u" in the function name.
             * 
             * The smaller binary operation for unsigned variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$<\f$ |
             * | :-----: | :-----: | :-----: |
             * | 0       | 0       | 0       |
             * | 0       | 1       | 1       |
             * | 1       | 0       | 0       |
             * | 1       | 1       | 0       |
             * 
             * From the truth table, we can conclude that A is only smaller than B if: \f$\overline{A}.B\f$
             * 
             * However, a smaller binary operation has different weights for all the bits in a variable, i.e, the more significant
             * the bit is, the more weight it has in the operation.
             * For example, in hypothetical terms, we are comparing 2 variables (A and B) both with 3 bits, A is smaller than B if:
             * ( A2 < B2 ) OR ( A2 = B2 AND A1 < B1 ) OR (A2 = B2 AND A1 = B1 AND A0 < B0 )
             * 
             * This can be translated into the following generic relation:
             * 
             * \begin{align*}
             *      A<B &= \overline{A_{n}}.B_{n} \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(\overline{A_{n-1}}.B_{n-1}) \\\
             *          &+ (...) \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(\overline{A_{n-1} \oplus B_{n-1}}).(...).(\overline{A_{1} \oplus B_{1}}).(\overline{A_{0}}.B_{0})
             * \end{align*}
             * 
             * Where \f$n\f$ is the number of bits.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable \f$Smaller\f$.
            **/
            void smaller_u(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief Evaluates if signed variable A is smaller than signed variable B.
             * 
             * -----
             * 
             * The circuit creation for a smaller operation is dependent on the sign of the input variables. As such, this
             * function is directed to an operation between two signed variables, thus the "s" in the function name.
             * 
             * The smaller binary operation for signed variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$<\f$ |
             * | :-----: | :-----: | :-----: |
             * | 0       | 0       | 0       |
             * | 0       | 1       | 0       |
             * | 1       | 0       | 1       |
             * | 1       | 1       | 0       |
             * 
             * From the truth table, we can conclude that A is only smaller than B if: \f$A.\overline{B}\f$
             * 
             * However, a smaller binary operation has different weights for all the bits in a variable, i.e, the more significant
             * the bit is, the more weight it has in the operation.
             * For example, in hypothetical terms, we are comparing 2 variables (A and B) both with 3 bits, A is smaller than B if:
             * ( A2 > B2 ) OR ( A2 = B2 AND A1 < B1 ) OR (A2 = B2 AND A1 = B1 AND A0 < B0 )
             * 
             * This can be translated into the following generic relation:
             * 
             * \begin{align*}
             *      A<B &= A_{n}.\overline{B_{n}} \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(\overline{A_{n-1}}.B_{n-1}) \\\
             *          &+ (...) \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(\overline{A_{n-1} \oplus B_{n-1}}).(...).(\overline{A_{1} \oplus B_{1}}).(\overline{A_{0}}.B_{0})
             * \end{align*}
             * 
             * Where \f$n\f$ is the number of bits.
             * 
             * If we take a look at a smaller unsigned operation, the signed operation has exactly the same expression, expect the
             * comparison between the most significant bits, which represent the sign.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output wire \f$Smaller\f$.
            **/
            void smaller_s(const Variable& in_a, const Variable& in_b, Wire& out);

            /**
             * @brief Evaluates if signed variable A is smaller than signed variable B.
             * 
             * -----
             * 
             * The circuit creation for a smaller operation is dependent on the sign of the input variables. As such, this
             * function is directed to an operation between two signed variables, thus the "s" in the function name.
             * 
             * The smaller binary operation for signed variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$<\f$ |
             * | :-----: | :-----: | :-----: |
             * | 0       | 0       | 0       |
             * | 0       | 1       | 0       |
             * | 1       | 0       | 1       |
             * | 1       | 1       | 0       |
             * 
             * From the truth table, we can conclude that A is only smaller than B if: \f$A.\overline{B}\f$
             * 
             * However, a smaller binary operation has different weights for all the bits in a variable, i.e, the more significant
             * the bit is, the more weight it has in the operation.
             * For example, in hypothetical terms, we are comparing 2 variables (A and B) both with 3 bits, A is smaller than B if:
             * ( A2 > B2 ) OR ( A2 = B2 AND A1 < B1 ) OR (A2 = B2 AND A1 = B1 AND A0 < B0 )
             * 
             * This can be translated into the following generic relation:
             * 
             * \begin{align*}
             *      A<B &= A_{n}.\overline{B_{n}} \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(\overline{A_{n-1}}.B_{n-1}) \\\
             *          &+ (...) \\\
             *          &+ (\overline{A_{n} \oplus B_{n}}).(\overline{A_{n-1} \oplus B_{n-1}}).(...).(\overline{A_{1} \oplus B_{1}}).(\overline{A_{0}}.B_{0})
             * \end{align*}
             * 
             * Where \f$n\f$ is the number of bits.
             * 
             * If we take a look at a smaller unsigned operation, the signed operation has exactly the same expression, expect the
             * comparison between the most significant bits, which represent the sign.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable \f$Smaller\f$.
            **/
            void smaller_s(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief Evaluates if unsigned variable A is greater or equal than unsigned variable B.
             * 
             * -----
             * 
             * The circuit creation for a greater or equal operation is dependent on the sign of the input variables. As such,
             * this function is directed to an operation between two unsigned variables, thus the "u" in the function name.
             * 
             * The greater or equal binary operation for unsigned variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$\geq\f$ |
             * | :-----: | :-----: | :--------: |
             * | 0       | 0       | 1          |
             * | 0       | 1       | 0          |
             * | 1       | 0       | 1          |
             * | 1       | 1       | 1          |
             * 
             * We can see that this truth table is a "smaller" unsigned operation truth table negated. As so, we can conclude that
             * we can achieve the greater or equal operation by negating the smaller operation result.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output wire \f$Greater\:or\:equal\f$.
            **/
            void greater_or_equal_u(const Variable& in_a, const Variable& in_b, Wire& out);

            /**
             * @brief Evaluates if unsigned variable A is greater or equal than unsigned variable B.
             * 
             * -----
             * 
             * The circuit creation for a greater or equal operation is dependent on the sign of the input variables. As such,
             * this function is directed to an operation between two unsigned variables, thus the "u" in the function name.
             * 
             * The greater or equal binary operation for unsigned variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$\geq\f$ |
             * | :-----: | :-----: | :--------: |
             * | 0       | 0       | 1          |
             * | 0       | 1       | 0          |
             * | 1       | 0       | 1          |
             * | 1       | 1       | 1          |
             * 
             * We can see that this truth table is a "smaller" unsigned operation truth table negated. As so, we can conclude that
             * we can achieve the greater or equal operation by negating the smaller operation result.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable \f$Greater\:or\:equal\f$.
            **/
            void greater_or_equal_u(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief Evaluates if signed variable A is greater or equal than signed variable B.
             * 
             * -----
             * 
             * The circuit creation for a greater or equal operation is dependent on the sign of the input variables. As such,
             * this function is directed to an operation between two signed variables, thus the "s" in the function name.
             * 
             * The greater or equal binary operation for signed variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$\geq\f$ |
             * | :-----: | :-----: | :--------: |
             * | 0       | 0       | 1          |
             * | 0       | 1       | 1          |
             * | 1       | 0       | 0          |
             * | 1       | 1       | 1          |
             * 
             * We can see that this truth table is a "smaller" signed operation truth table negated. As so, we can conclude that
             * we can achieve the greater or equal operation by negating the smaller operation result.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output wire \f$Greater\:or\:equal\f$.
            **/
            void greater_or_equal_s(const Variable& in_a, const Variable& in_b, Wire& out);

            /**
             * @brief Evaluates if signed variable A is greater or equal than signed variable B.
             * 
             * -----
             * 
             * The circuit creation for a greater or equal operation is dependent on the sign of the input variables. As such,
             * this function is directed to an operation between two signed variables, thus the "s" in the function name.
             * 
             * The greater or equal binary operation for signed variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$\geq\f$ |
             * | :-----: | :-----: | :--------: |
             * | 0       | 0       | 1          |
             * | 0       | 1       | 1          |
             * | 1       | 0       | 0          |
             * | 1       | 1       | 1          |
             * 
             * We can see that this truth table is a "smaller" signed operation truth table negated. As so, we can conclude that
             * we can achieve the greater or equal operation by negating the smaller operation result.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable \f$Greater\:or\:equal\f$.
            **/
            void greater_or_equal_s(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief Evaluates if unsigned variable A is smaller or equal than unsigned variable B.
             * 
             * -----
             * 
             * The circuit creation for a smaller or equal operation is dependent on the sign of the input variables. As such,
             * this function is directed to an operation between two unsigned variables, thus the "u" in the function name.
             * 
             * The smaller or equal binary operation for unsigned variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$\leq\f$ |
             * | :-----: | :-----: | :--------: |
             * | 0       | 0       | 1          |
             * | 0       | 1       | 1          |
             * | 1       | 0       | 0          |
             * | 1       | 1       | 1          |
             * 
             * We can see that this truth table is a "greater" unsigned operation truth table negated. As so, we can conclude that
             * we can achieve the smaller or equal operation by negating the greater operation result.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output wire \f$Smaller\:or\:equal\f$.
            **/
            void smaller_or_equal_u(const Variable& in_a, const Variable& in_b, Wire& out);

            /**
             * @brief Evaluates if unsigned variable A is smaller or equal than unsigned variable B.
             * 
             * -----
             * 
             * The circuit creation for a smaller or equal operation is dependent on the sign of the input variables. As such,
             * this function is directed to an operation between two unsigned variables, thus the "u" in the function name.
             * 
             * The smaller or equal binary operation for unsigned variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$\leq\f$ |
             * | :-----: | :-----: | :--------: |
             * | 0       | 0       | 1          |
             * | 0       | 1       | 1          |
             * | 1       | 0       | 0          |
             * | 1       | 1       | 1          |
             * 
             * We can see that this truth table is a "greater" unsigned operation truth table negated. As so, we can conclude that
             * we can achieve the smaller or equal operation by negating the greater operation result.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable \f$Smaller\:or\:equal\f$.
            **/
            void smaller_or_equal_u(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief Evaluates if signed variable A is smaller or equal than signed variable B.
             * 
             * -----
             * 
             * The circuit creation for a smaller or equal operation is dependent on the sign of the input variables. As such,
             * this function is directed to an operation between two signed variables, thus the "s" in the function name.
             * 
             * The smaller or equal binary operation for signed variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$\leq\f$ |
             * | :-----: | :-----: | :--------: |
             * | 0       | 0       | 1          |
             * | 0       | 1       | 0          |
             * | 1       | 0       | 1          |
             * | 1       | 1       | 1          |
             * 
             * We can see that this truth table is a "greater" signed operation truth table negated. As so, we can conclude that
             * we can achieve the smaller or equal operation by negating the greater operation result.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output wire \f$Smaller\:or\:equal\f$.
            **/
            void smaller_or_equal_s(const Variable& in_a, const Variable& in_b, Wire& out);

            /**
             * @brief Evaluates if signed variable A is smaller or equal than signed variable B.
             * 
             * -----
             * 
             * The circuit creation for a smaller or equal operation is dependent on the sign of the input variables. As such,
             * this function is directed to an operation between two signed variables, thus the "s" in the function name.
             * 
             * The smaller or equal binary operation for signed variables is represented by the following truth table:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$\leq\f$ |
             * | :-----: | :-----: | :--------: |
             * | 0       | 0       | 1          |
             * | 0       | 1       | 0          |
             * | 1       | 0       | 1          |
             * | 1       | 1       | 1          |
             * 
             * We can see that this truth table is a "greater" signed operation truth table negated. As so, we can conclude that
             * we can achieve the smaller or equal operation by negating the greater operation result.
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out Output variable \f$Smaller\:or\:equal\f$.
            **/
            void smaller_or_equal_s(const Variable& in_a, const Variable& in_b, Variable& out);

            /**
             * @brief Compares two unsigned binary numbers.
             * 
             * -----
             * 
             * A magnitude digital comparator is a combinational circuit that compares two digital or binary numbers in order to
             * find out whether one binary number is equal, less than, or greater than the other binary number.
             * 
             * The circuit creation for a comparator operation is dependent on the sign of the input variables. As such, this
             * function is directed to an operation between two unsigned variables, thus the "u" in the function name.
             * 
             * An unsigned comparator truth table is the fusion of the equal, greater, and smaller truth tables for unsigned
             * variables:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$=\f$ | \f$>\f$ | \f$<\f$ |
             * | :-----: | :-----: | :-----: | :-----: | :-----: |
             * | 0       | 0       | 1       | 0       | 0       |
             * | 0       | 1       | 0       | 0       | 1       |
             * | 1       | 0       | 0       | 1       | 0       |
             * | 1       | 1       | 1       | 0       | 0       |
             * 
             * Thus, the comparator expression is all the three expressions from equal, greater, and smaller unsigned operations
             * (check operations documentation).
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out_e Output wire \f$Equal\f$.
             * @param out_g Output wire \f$Greater\f$.
             * @param out_s Output wire \f$Smaller\f$.
            **/
            void comparator_u(const Variable& in_a, const Variable& in_b, Wire& out_e, Wire& out_g, Wire &out_s);
            
            /**
             * @brief Compares two unsigned binary numbers.
             * 
             * -----
             * 
             * A magnitude digital comparator is a combinational circuit that compares two digital or binary numbers in order to
             * find out whether one binary number is equal, less than, or greater than the other binary number.
             * 
             * The circuit creation for a comparator operation is dependent on the sign of the input variables. As such, this
             * function is directed to an operation between two unsigned variables, thus the "u" in the function name.
             * 
             * An unsigned comparator truth table is the fusion of the equal, greater, and smaller truth tables for unsigned
             * variables:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$=\f$ | \f$>\f$ | \f$<\f$ |
             * | :-----: | :-----: | :-----: | :-----: | :-----: |
             * | 0       | 0       | 1       | 0       | 0       |
             * | 0       | 1       | 0       | 0       | 1       |
             * | 1       | 0       | 0       | 1       | 0       |
             * | 1       | 1       | 1       | 0       | 0       |
             * 
             * Thus, the comparator expression is all the three expressions from equal, greater, and smaller unsigned operations
             * (check operations documentation).
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out_e Output variable \f$Equal\f$.
             * @param out_g Output variable \f$Greater\f$.
             * @param out_s Output variable \f$Smaller\f$.
            **/
            void comparator_u(const Variable& in_a, const Variable& in_b, Variable& out_e, Variable& out_g, Variable &out_s);

            /**
             * @brief Compares two signed binary numbers.
             * 
             * -----
             * 
             * A magnitude digital comparator is a combinational circuit that compares two digital or binary numbers in order to
             * find out whether one binary number is equal, less than, or greater than the other binary number.
             * 
             * The circuit creation for a comparator operation is dependent on the sign of the input variables. As such, this
             * function is directed to an operation between two signed variables, thus the "s" in the function name.
             * 
             * A signed comparator truth table is the fusion of the equal, greater, and smaller truth tables for signed variables:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$=\f$ | \f$>\f$ | \f$<\f$ |
             * | :-----: | :-----: | :-----: | :-----: | :-----: |
             * | 0       | 0       | 1       | 0       | 0       |
             * | 0       | 1       | 0       | 1       | 0       |
             * | 1       | 0       | 0       | 0       | 1       |
             * | 1       | 1       | 1       | 0       | 0       |
             * 
             * Thus, the comparator expression is all the three expressions from equal, greater, and smaller signed operations
             * (check operations documentation).
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out_e Output variable \f$Equal\f$.
             * @param out_g Output variable \f$Greater\f$.
             * @param out_s Output variable \f$Smaller\f$.
            **/
            void comparator_s(const Variable& in_a, const Variable& in_b, Wire& out_e, Wire& out_g, Wire &out_s);

            /**
             * @brief Compares two signed binary numbers.
             * 
             * -----
             * 
             * A magnitude digital comparator is a combinational circuit that compares two digital or binary numbers in order to
             * find out whether one binary number is equal, less than, or greater than the other binary number.
             * 
             * The circuit creation for a comparator operation is dependent on the sign of the input variables. As such, this
             * function is directed to an operation between two signed variables, thus the "s" in the function name.
             * 
             * A signed comparator truth table is the fusion of the equal, greater, and smaller truth tables for signed variables:
             * 
             * | \f$A\f$ | \f$B\f$ | \f$=\f$ | \f$>\f$ | \f$<\f$ |
             * | :-----: | :-----: | :-----: | :-----: | :-----: |
             * | 0       | 0       | 1       | 0       | 0       |
             * | 0       | 1       | 0       | 1       | 0       |
             * | 1       | 0       | 0       | 0       | 1       |
             * | 1       | 1       | 1       | 0       | 0       |
             * 
             * Thus, the comparator expression is all the three expressions from equal, greater, and smaller signed operations
             * (check operations documentation).
             * 
             * @param in_a Input variable \f$A\f$.
             * @param in_b Input variable \f$B\f$.
             * @param out_e Output wire \f$Equal\f$.
             * @param out_g Output wire \f$Greater\f$.
             * @param out_s Output wire \f$Smaller\f$.
            **/
            void comparator_s(const Variable& in_a, const Variable& in_b, Variable& out_e, Variable& out_g, Variable &out_s);
        };

        /**
         * @brief Circuit generator class for Bristol format circuits.
         * 
         * -----
         * 
         * This class extends all the core functionality of the circuit generator abstract class and it was developed to target
         * the generation of boolean circuits with [Bristol](https://homes.esat.kuleuven.be/~nsmart/MPC/) format (check link to
         * undestand the specific format features for Bristol).
        **/
        class BristolCircuitGenerator : public CircuitGenerator
        {
        protected:
            /**
             * @brief Writes the header section of the circuit into a file.
             * 
             * -----
             * 
             * This function overrides the behavior from the original virtual function from the circuit generator abstract class.
             * It makes writes the header section with the correct Bristol format.
             * 
             * @param file File to write.
            **/
            virtual void _write_header(std::ofstream& file) override;
        
        public:
            /** @brief Default construction of a new Bristol Circuit Generator object is deleted. **/
            BristolCircuitGenerator() = delete;

            /**
             * @brief Construct a new Bristol Circuit Generator object for a specified circuit name and location.
             * 
             * -----
             * 
             * In the end, the generated circuit file will have the name of the inserted circuit name and located into the
             * inserted location.
             * 
             * @note By default, the directory to the circuits is located at a circuits directory that should be in the same level
             * from where the executable is being executed.
             * 
             * @param circuit_name Name of the circuit.
             * @param circuits_directory Location of the circuit.
            **/
            BristolCircuitGenerator(const std::string &circuit_name, const std::string &circuits_directory = "circuits");
        };

        /**
         * @brief Circuit generator class for Libscapi format circuits.
         * 
         * -----
         * 
         * This class extends all the core functionality of the circuit generator abstract class and it was developed to target
         * the generation of boolean circuits with [Libscapi](https://libscapi.readthedocs.io/en/latest/circuits.html) format
         * (check link to undestand the specific format features for Libscapi).
         * 
         * @note Libscapi is an open source secure computation library that has it's own format to write/read boolean circuits.
         * Here is their [github](https://github.com/cryptobiu/libscapi).
        **/
        class LibscapiCircuitGenerator : public CircuitGenerator
        {
        protected:
            /**
             * @brief Writes the header section of the circuit into a file.
             * 
             * -----
             * 
             * This function overrides the behavior from the original virtual function from the circuit generator abstract class.
             * It makes writes the header section with the correct Libscapi format.
             * 
             * @param file File to write.
            **/
            virtual void _write_header(std::ofstream& file) override;
        
        public:
            /** @brief Default construction of a new Libscapi Circuit Generator object is deleted. **/
            LibscapiCircuitGenerator() = delete;
            
            /**
             * @brief Construct a new Libscapi Circuit Generator object for a specified circuit name and location.
             * 
             * -----
             * 
             * In the end, the generated circuit file will have the name of the inserted circuit name and located into the
             * inserted location.
             * 
             * @note By default, the directory to the circuits is located at a circuits directory that should be in the same level
             * from where the executable is being executed.
             * 
             * @param circuit_name Name of the circuit.
             * @param circuits_directory Location of the circuit.
            **/
            LibscapiCircuitGenerator(const std::string &circuit_name, const std::string &circuits_directory = "circuits");
        };
    }
}
