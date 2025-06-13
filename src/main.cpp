#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <array>
#include <sys/types.h>

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

void print_promt()
{
    std::cout << "db > ";
};

void read_input(InputBuffer* input_buffer)
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

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum { 
    PREPARE_SUCCESS, 
    PREPARE_UNRECOGNIZED_STATEMENT 
} PrepareResult;

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
  if (strcmp(input_buffer->get_buffer(), ".exit") == 0) {
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

constexpr uint32_t COLUMN_USERNAME_SIZE = 32;
constexpr uint32_t COLUMN_EMAIL_SIZE = 256;

struct Row
{
    uint32_t id;
    std::array<char, COLUMN_USERNAME_SIZE> username;
    std::array<char, COLUMN_EMAIL_SIZE> email;
};

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute);

constexpr uint32_t id_size = size_of_attribute(Row, id);
constexpr uint32_t username_size = size_of_attribute(Row, username);
constexpr uint32_t email_size = size_of_attribute(Row, email);
constexpr uint32_t id_offset = 0;
constexpr uint32_t username_offset = id_offset + id_size;
constexpr uint32_t email_offset = username_offset + username_size;
constexpr uint32_t row_size = username_size + email_size + id_size;

void serialize_row(const Row* source, std::byte* destination)
{
    // if (destination < row_size)
    // {
    //     throw std::runtime_error("Buffer too small for serialization");
    // }

    memcpy(destination + id_offset, &(source->id), id_size);
    memcpy(destination + username_offset, &(source->username), username_size);
    memcpy(destination + email_offset, &(source->email), email_size);
}

void deserialize_row(const std::byte* source, Row* destination)
{
    memcpy(&(destination->id), source + id_offset, id_size);
    memcpy(&(destination->username), source + username_offset, username_size);
    memcpy(&(destination->email), source + email_offset, email_size);
}

const uint32_t page_size = 4096;
constexpr uint32_t table_max_pages = 100;
const uint32_t rows_per_page = page_size / row_size;
const uint32_t table_max_rows = rows_per_page * table_max_pages;

struct Table {
public:
    uint32_t num_rows;
    std::byte* pages[table_max_pages];
};

typedef enum { 
    STATEMENT_INSERT, 
    STATEMENT_SELECT 
} StatementType;


class Statement
{
public:
    // TODO: place to private
    StatementType type;
    Row row_to_insert;
private:
};

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->get_buffer(), "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        int args_assigned = scanf(input_buffer->get_buffer(), 
            "insert %d %s %s", &(statement->row_to_insert.id), 
            statement->row_to_insert.username, statement->row_to_insert.email);
        if (args_assigned < 3)
        {
            std::cout << "Error: less than 3 arguments\n";
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->get_buffer(), "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement)
{
    switch (statement->type)
    {
        case(STATEMENT_INSERT):
            std::cout << "Insert statement has been detected.\n";
            break;
        case(STATEMENT_SELECT):
            std::cout << "Select statement has been detected.\n";
            break;
    }  
}

int main(int argc, char* argv[])
{
    InputBuffer* input_buffer = new InputBuffer();
    while (true)
    {
        print_promt();
        read_input(input_buffer);

        if (input_buffer->get_buffer()[0] == '.')
        {
            switch(do_meta_command(input_buffer))
            {
                case (META_COMMAND_SUCCESS):    
                        continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    std::cout << "Unrecognized command " << input_buffer->get_buffer() << std::endl;
                    continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) 
        {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                std::cout << "Unrecognized keyword at start of " << input_buffer->get_buffer() << std::endl;
                continue;
        }

        execute_statement(&statement);
        std::cout << "Executed." << std::endl;
    }
}