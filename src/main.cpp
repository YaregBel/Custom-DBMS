#include <chrono>
#include <thread>

#include "statement.hpp"

void print_promt()
{
    std::cout << "db > ";
};

bool isMetaCommand(InputBuffer* input_buffer) 
{
    if (input_buffer->buffer()[0][0] == '.') 
    {
        switch(StatementManipulator::do_meta_command(input_buffer)) 
        {
            case META_COMMAND_SUCCESS:
                return true;
            case META_COMMAND_UNRECOGNIZED_COMMAND:
                std::cout << "Unrecognized command " << input_buffer->buffer()[0] << std::endl;
                return true;
        }
    }
    return false;
}

std::pair<Statement, bool> prepareStatementForExecution(InputBuffer* input_buffer)
{
    Statement statement;
    // TODO: get rid of output argument
    switch (StatementManipulator::prepare_statement(input_buffer, &statement)) 
    {
        case (PREPARE_SUCCESS):
            return {statement, false};
        case (PREPARE_UNRECOGNIZED_STATEMENT):
            std::cout << "Unrecognized keyword " << input_buffer->buffer()[0] << "\n";
            return {statement, true};
        case(PREPARE_SYNTAX_ERROR):
            std::cout << "Syntax error! Couldn't parse statement.\n";
            return {statement, true};
    }

    return {statement, true};
}

void executeStatement(Statement* statement, Table* table) {
    switch(StatementManipulator::execute_statement(statement, table)) {
        case EXECUTE_SUCCESS:
            std::cout << "Executed.\n";
            break;
        case EXECUTE_TABLE_FULL:
            std::cout << "Error: Table full.\n";
            break;
        case EXECUTE_ERROR:
            std::cout << "Execute error!\n";
            break;
    }
}

int main(int argc, char* argv[])
{
    Table* table = new Table("database_name.db");
    InputBuffer* input_buffer = new InputBuffer();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    while (true)
    {
        print_promt();
        InputManipulator::read_input(input_buffer);

        if(isMetaCommand(input_buffer)) 
        {
            continue;
        }

        auto [statement, shouldSkip] = prepareStatementForExecution(input_buffer);
        if (shouldSkip) 
        {
            continue;
        }
        
        StatementManipulator::execute_statement(&statement, table);
    }
    delete(table);
    delete(input_buffer);
}