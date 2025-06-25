cmake_minimum_required(VERSION 3.10)
project(CustomDBMS)

# Минимальная версия, для работы проекта
set(CMAKE_CXX_STANDARD 17)          
set(CMAKE_CXX_STANDARD_REQUIRED ON)  

# Куда будут попадать бинарники
set(BIN_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin" CACHE PATH "Directory for output binaries")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})

add_executable(db src/main.cpp)

find_package(Catch2 3 REQUIRED)

add_executable(tests src/tests/statements_test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR NOT CMAKE_BUILD_TYPE)

    target_compile_options(tests PRIVATE
        $<$<CONFIG:Debug>:-g3 -O0>    
    )

    target_link_options(tests PRIVATE
        $<$<CONFIG:Debug>:-g3>    
    )

endif()