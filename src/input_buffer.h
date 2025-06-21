#include <iostream>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

class InputBuffer
{
public:
    InputBuffer()
    {
        this->buffer = nullptr;
        this->buffer_length = 0;
        this->input_length = 0;
    }
    InputBuffer(char* buffer, size_t buffer_length, ssize_t input_length)
    {
        this->buffer = buffer;
        this->buffer_length = buffer_length;
        this->input_length = input_length;
    }

    ~InputBuffer() = default;

    char* get_buffer()
    {
        return this->buffer;
    }
    size_t get_buffer_length()
    {
        return this->buffer_length;
    }
    ssize_t get_input_length()
    {
        return this->input_length;
    }

    void set_buffer(char* bufferToSet)
    {
        this->buffer = bufferToSet;
    }
    void set_buffer_length(size_t lengthToSet)
    {
        this->buffer_length = lengthToSet;
    }
    void set_input_length(ssize_t lengthToSet)
    {
        this->input_length = lengthToSet;
    }   

private:
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
};

class InputManipulator
{
public:
    static void read_input(InputBuffer* input_buffer)
    {
        char* buffer_info = NULL;
        size_t buffer_len = 0;
        ssize_t bytes_read = getline(&buffer_info, &buffer_len, stdin);

        if (bytes_read <= 0)
        {
            std::cout << "Error reading input\n";
            exit(EXIT_FAILURE);
        }

        ssize_t input_bytes = bytes_read - 1;
        buffer_info[input_bytes] = 0;

        input_buffer->set_buffer(buffer_info);
        input_buffer->set_buffer_length(buffer_len);
        input_buffer->set_input_length(input_bytes);
    }
};