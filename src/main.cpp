#include <chrono>
#include <thread>

#include "statement.h"

void print_promt()
{
    std::cout << "db > ";
};

int main(int argc, char* argv[])
{
    Table* table = new Table();
    InputBuffer* input_buffer = new InputBuffer();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    while (true)
    {
        print_promt();
        InputManipulator::read_input(input_buffer);

        if (input_buffer->buffer()[0][0] == '.')
        {
            switch(StatementManipulator::do_meta_command(input_buffer))
            {
                case (META_COMMAND_SUCCESS):    
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    std::cout << "Unrecognized command " << input_buffer->buffer()[0] << std::endl;
                    continue;
            }
        }

        Statement statement;
        switch (StatementManipulator::prepare_statement(input_buffer, &statement)) 
        {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                std::cout << "Unrecognized keyword: " << input_buffer->buffer()[0] << "\n";
                continue;
            case(PREPARE_SYNTAX_ERROR):
                std::cout << "Syntax error! Couldn't parse statement.\n";
                continue;
        }

        //StatementManipulator::execute_statement(&statement, table);
        switch(StatementManipulator::execute_statement(&statement, table))
        {
            case (EXECUTE_SUCCESS):
                std::cout << "Executed.\n";
                break;
            case (EXECUTE_TABLE_FULL):
                std::cout << "Error: Table full.\n";
                break;
            case (EXECUTE_ERROR):
                std::cout << "Execute error!\n";
                break;
        }
    }

    delete(table);
    delete(input_buffer);
}