#include <iostream>
#include <vector>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

class InputBuffer
{
public:
    InputBuffer() = default;
    InputBuffer(std::vector<std::string> input) : _buffer(input), _input_size(input.size()) {};

    void set_buffer(std::vector<std::string> input)
    {
        this->_buffer = input;
        this->_input_size = input.size();
    }

    const std::vector<std::string>& buffer() const
    {
        return this->_buffer;
    }
    const size_t size() const
    {
        return this->_input_size;
    }

    ~InputBuffer() = default;
private:
    std::vector<std::string> _buffer;
    size_t _input_size = 0;
};

class InputManipulator
{
public:
    static void read_input(InputBuffer* input_buffer)
    {
        std::string line;
        std::getline(std::cin, line);
        
        std::string token;
        std::vector<std::string> tokens;
        
        for (auto ch : line) 
        {
            if (ch != ' ') 
            {
                token += ch;
            }  
            else 
            {
                tokens.push_back(token);
                token.clear();
            }
        }
        tokens.push_back(token);
        token.clear();
        input_buffer->set_buffer(tokens);

        if (input_buffer->size() <= 0)
        {
            std::cout << "Error: reading input\n";
            std::cout << "Error: Содержимое tokens[0]: " << input_buffer->buffer()[0] << "\n";
            exit(EXIT_FAILURE);
        }
    }
};