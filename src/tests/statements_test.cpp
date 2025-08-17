#include <chrono>
#include <cstddef>
#include <thread>

#include "../statement.h"
#include <catch2/catch_test_macros.hpp>

#define CATCH_CONFIG_MAIN

TEST_CASE( "check if \".exit\" work", "[.exit]" ) 
{
    Table* table = new Table();
    InputBuffer* input_buffer = new InputBuffer();

    std::vector<std::string> exitCom = {".exit"};
    input_buffer->set_buffer(exitCom);
    REQUIRE(input_buffer->buffer() == exitCom);
}

TEST_CASE( "check \"insert\" works", "[insert]" ) 
{
    Table* table = new Table();
    InputBuffer* input_buffer = new InputBuffer();

    Statement statement;

    SECTION( "Insert command with correct input. [Positive]" ) 
    {
        input_buffer->set_buffer({"insert", "1", "yareg", "yareg@gmail.com"});

        REQUIRE( StatementManipulator::prepare_statement(input_buffer, 
            &statement) == PREPARE_SUCCESS );
        REQUIRE( StatementManipulator::execute_statement(&statement, 
            table) == EXECUTE_SUCCESS);
    }

    SECTION( "Insert command with correct input. [Positive]" ) 
    {
        input_buffer->set_buffer({"insert", "1", "yareg", "yareg@gmail.com"});
        
        REQUIRE( StatementManipulator::prepare_statement(input_buffer, 
            &statement) == PREPARE_SUCCESS );
        REQUIRE( StatementManipulator::execute_statement(&statement, 
            table) == EXECUTE_SUCCESS);
    }

    SECTION( "Insert command with incorrect input. [Negative]" ) 
    {
        input_buffer->set_buffer({"insert", "1", "yareg"});
        REQUIRE( StatementManipulator::prepare_statement(input_buffer, 
            &statement) == PREPARE_UNRECOGNIZED_STATEMENT );
    }

        SECTION( "Insert command with incorrect ID format. [Negative]" ) 
    {
        input_buffer->set_buffer({"insert", "a", "yareg", "yareg@gmail.com"});
        
        REQUIRE( StatementManipulator::prepare_statement(input_buffer, 
            &statement) == PREPARE_SYNTAX_ERROR );
        REQUIRE( StatementManipulator::execute_statement(&statement, 
            table) == EXECUTE_ERROR);
    }

        SECTION( "Insert command with excessive username length. [Negative]" ) 
    {
        input_buffer->set_buffer({"insert", "1", "yaregyaregyaregyaregyaregyaregyaregyareg", "yareg@gmail.com"});
        
        REQUIRE( StatementManipulator::prepare_statement(input_buffer, 
            &statement) == PREPARE_SYNTAX_ERROR );
        REQUIRE( StatementManipulator::execute_statement(&statement, 
            table) == EXECUTE_ERROR);
    }
}
