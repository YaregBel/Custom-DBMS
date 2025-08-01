#include <chrono>
#include <cstddef>
#include <thread>

#include "../statement.h"
#include <catch2/catch_test_macros.hpp>

#define CATCH_CONFIG_MAIN

TEST_CASE( "vectors can be sized and resized", "[vector]" ) 
{
    Table* table = new Table();
    InputBuffer* input_buffer = new InputBuffer();

    SECTION( "Statement .exit must processed correctly" ) 
    {
        std::vector<std::string> exitCom = {".exit"};
        input_buffer->set_buffer(exitCom);
        REQUIRE(input_buffer->buffer() == exitCom);
    }

    SECTION( "Statement insert should work correctly" ) 
    {
        Statement statement;

        input_buffer->set_buffer({"insert", "1", "yareg", "yareg@gmail.com"});
        REQUIRE( StatementManipulator::prepare_statement(input_buffer, 
            &statement) == PREPARE_SUCCESS );
        REQUIRE( StatementManipulator::execute_statement(&statement, 
            table) == EXECUTE_SUCCESS);
            
        input_buffer->set_buffer({"insert", "1", "yareg"});
        REQUIRE( StatementManipulator::prepare_statement(input_buffer, 
            &statement) == PREPARE_SYNTAX_ERROR );
    }
}