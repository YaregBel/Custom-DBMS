#include "statement.h"

void print_promt()
{
    std::cout << "db > ";
};

int main(int argc, char* argv[])
{
    InputBuffer* input_buffer = new InputBuffer();
    while (true)
    {
        print_promt();
        InputManipulator::read_input(input_buffer);

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
        switch (StatementManipulator::prepare_statement(input_buffer, &statement)) 
        {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                std::cout << "Unrecognized keyword at start of " << input_buffer->get_buffer() << "\n";
                continue;
            case(PREPARE_SYNTAX_ERROR):
                std::cout << "Syntax error!\n";
        }

        StatementManipulator::execute_statement(&statement);
        std::cout << "Executed." << std::endl;
    }
}