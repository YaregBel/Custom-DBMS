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
        bool commandExistance = isCommandExist(input_buffer);
        if (commandExistance)
        {
            PrepareResult result = fillStatement(input_buffer, statement);

            return result;
        }

        return PREPARE_UNRECOGNIZED_STATEMENT;
    }

    static bool isCommandExist(InputBuffer* input_buffer)
    {
        std::vector<std::string> sqlCommands{"insert", "select"};
        for (auto sqlCommand : sqlCommands)
        {
            if (input_buffer->buffer()[0] == sqlCommand)
            {
                return true;
            }
        }
        return false;
    }

    static PrepareResult fillStatement(InputBuffer* input_buffer, Statement* statement)
    {
        std::string sqlCommand = input_buffer->buffer()[0];
        if (sqlCommand == "insert")
        {
            int args_assigned = input_buffer->size();
                
            if (args_assigned == 4)
            {
                if (!isIdValid(input_buffer, statement))
                {
                    return PREPARE_SYNTAX_ERROR;
                }

                fillInsertStatement(input_buffer, statement);

                return PREPARE_SUCCESS;
            }
        }
        else if (sqlCommand == "select")
        {
            fillSelectStatement(input_buffer, statement);
            
            return PREPARE_SUCCESS;
        }

        return PREPARE_UNRECOGNIZED_STATEMENT;
    }

    static bool isIdValid(InputBuffer* input_buffer, Statement* statement)
    {
        try
        {
            statement->row_to_insert.setStatementId(input_buffer->buffer()[1]);
            return true;
        }
        catch(std::invalid_argument)
        {
            std::cout << "Invalid ID format.\n";
            return false;
        }
    }

    static void fillInsertStatement(InputBuffer* input_buffer, Statement* statement)
    {
        statement->row_to_insert.setUsernameFromString(input_buffer->buffer()[2]);
        statement->row_to_insert.setEmailFromString(input_buffer->buffer()[3]);

        statement->statementLength = 3;
        statement->type = STATEMENT_INSERT;
    }

    static void fillSelectStatement(InputBuffer* input_buffer, Statement* statement)
    {
        statement->type = STATEMENT_SELECT;
        statement->statementLength = 1;
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

        return EXECUTE_ERROR;
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