#include <cstdint>
#include <iostream>
#include <iterator>
#include <stdexcept>
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

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL, 
    EXECUTE_ERROR
} ExecuteResult;

class Statement
{
public:
    // TODO: place to private
    StatementType type;
    Row row_to_insert;
    int statementLength;
private:

};

class StatementManipulator
{
public:
    static MetaCommandResult do_meta_command(InputBuffer* input_buffer) 
    {
        if (input_buffer->buffer()[0] == ".exit") {
            exit(EXIT_SUCCESS);
        } else {
            return META_COMMAND_UNRECOGNIZED_COMMAND;
        }
    }

    static PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) 
    {
        if (input_buffer->buffer()[0] == "insert") 
        {    
            std::cout << "Debug: Input buffer = '" << input_buffer->buffer()[0] << "'\n";
            
            int args_assigned = input_buffer->size();
            
            if (args_assigned == 4)
            {
                try
                {
                    statement->row_to_insert.setStatementId(input_buffer->buffer()[1]);
                }
                catch(std::invalid_argument)
                {
                    std::cout << "Invalid ID format.\n";
                    return PREPARE_SYNTAX_ERROR;
                }
                
                statement->row_to_insert.setUsernameFromString(input_buffer->buffer()[2]);
                statement->row_to_insert.setEmailFromString(input_buffer->buffer()[3]);

                statement->statementLength = 3;

                statement->type = STATEMENT_INSERT;
                return PREPARE_SUCCESS;
            }
            else 
            {
                std::cout << "Error: you should use 3 arguments\n";
                return PREPARE_SYNTAX_ERROR;
            }
        }
        if (input_buffer->buffer()[0] == "select") {
            statement->type = STATEMENT_SELECT;
            statement->statementLength = 1;
            return PREPARE_SUCCESS;
        }

        return PREPARE_UNRECOGNIZED_STATEMENT;
    }

    static ExecuteResult execute_statement(Statement* statement, Table *table)
    {
        switch (statement->type)
        {
            case(STATEMENT_INSERT):
                return execute_insert(statement, table);
            case(STATEMENT_SELECT):
                return execute_select(statement, table);
        }  
    }

    static bool checkStatementLength(Statement* statement, int requiredLength)
    {
        if (statement->statementLength == requiredLength)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    static ExecuteResult execute_insert(Statement* statement, Table* table)
    {
        if (checkStatementLength(statement, 3))
        {
            if (table->num_rows >= table_max_rows)
            {
                return EXECUTE_TABLE_FULL;
            }
            
            Row* row_to_insert = &(statement->row_to_insert);
            RowManipulator::serialize_row(row_to_insert, TableManipulator::row_slot(table, table->num_rows));
            table->num_rows += 1;
            
            // TODO: В методе не хватает проверки на успешность выполнения запроса
            return EXECUTE_SUCCESS;
        }
        else 
        {
            return EXECUTE_ERROR;
        }
    }

    static ExecuteResult execute_select(Statement* statement, Table* table)
    {
        if (table->num_rows <= 0)
        {
            std::cout << "Table is empty.\n";
        }

        Row row_to_select;
        for (uint32_t i = 0; i < table->num_rows; i++)
        {
            RowManipulator::deserialize_row(TableManipulator::row_slot(table, i), &row_to_select);
            RowManipulator::print_row(&row_to_select);
        }
        return EXECUTE_SUCCESS;
    }
};