#include <chrono>
#include <cstddef>
#include <thread>

#include "../statement.h"
#include <catch2/catch_test_macros.hpp>

#define CATCH_CONFIG_MAIN

TEST_CASE( "vectors can be sized and resized", "[vector]" ) {
    // This setup will be done 4 times in total, once for each section
    Table* table = new Table();
    InputBuffer* input_buffer = new InputBuffer();

    SECTION( "Statement .exit must processed correctly" ) 
    {
        std::vector<std::string> exitCom = {".exit"};

        input_buffer->set_buffer(exitCom);

        REQUIRE(input_buffer->buffer() == exitCom);
        
        MetaCommandResult meta_command_result = StatementManipulator::do_meta_command(input_buffer);
        REQUIRE( meta_command_result != META_COMMAND_UNRECOGNIZED_COMMAND);

        std::cout << "Проверка выражений прошла!" << std::endl;
    }

    // SECTION("First row") {
    //     std::byte* slot = TableManipulator::row_slot(table, 0);
    //     REQUIRE(slot != nullptr);
    // }
    
    // SECTION( "Statement insert should work correctly" ) {
    //     input_buffer->set_buffer("insert 1 yareg yareg@mail.ru");
        
    //     InputManipulator::read_input(input_buffer);
        
    //     Statement statement;
    //     REQUIRE( StatementManipulator::prepare_statement(input_buffer, 
    //         &statement) == PREPARE_SUCCESS );
    //     REQUIRE( StatementManipulator::execute_statement(&statement, 
    //         table) == EXECUTE_SUCCESS);
    // }
}