#include <cstdint>
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
private:
};

class StatementManipulator
{
public:
    static MetaCommandResult do_meta_command(InputBuffer* input_buffer) 
    {
        if (strcmp(input_buffer->get_buffer(), ".exit") == 0) {
            exit(EXIT_SUCCESS);
        } else {
            return META_COMMAND_UNRECOGNIZED_COMMAND;
        }
    }

    static PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) 
    {
        if (strncmp(input_buffer->get_buffer(), "insert", 6) == 0) {
            std::cout << "Debug: Input buffer = '" << input_buffer->get_buffer() << "'\n";
            int args_assigned = sscanf(input_buffer->get_buffer(), 
                "insert %d %31s %255s", &(statement->row_to_insert.id), 
                statement->row_to_insert.username, statement->row_to_insert.email);
            statement->type = STATEMENT_INSERT;

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

    static ExecuteResult execute_insert(Statement* statement, Table* table)
    {
        if (table->num_rows >= table_max_rows)
        {
            return EXECUTE_TABLE_FULL;
        }
        
        Row* row_to_insert = &(statement->row_to_insert);
        RowManipulator::serialize_row(row_to_insert, TableManipulator::row_slot(table, table->num_rows));
        table->num_rows += 1;
        
        return EXECUTE_SUCCESS;
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