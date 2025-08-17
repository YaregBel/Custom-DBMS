#include <chrono>
#include <thread>

#include "statement.h"

int main(int argc, char* argv[])
{
    Table* table = new Table();
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