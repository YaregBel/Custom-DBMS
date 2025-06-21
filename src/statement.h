#include <iostream>
#include "input_buffer.h"
#include "table.h"

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum { 
    PREPARE_SUCCESS, 
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR
} PrepareResult;

typedef enum { 
    STATEMENT_INSERT, 
    STATEMENT_SELECT 
} StatementType;

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
  if (strcmp(input_buffer->get_buffer(), ".exit") == 0) {
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

class Statement
{
public:
    // TODO: place to private
    StatementType type;
    Row row_to_insert;
private:
};

class StatementManipulator
{
public:
    static PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) 
    {
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

    static void execute_statement(Statement* statement)
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
};