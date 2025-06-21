#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <array>
#include <sys/types.h>

constexpr uint32_t COLUMN_USERNAME_SIZE = 32;
constexpr uint32_t COLUMN_EMAIL_SIZE = 256;
constexpr uint32_t page_size = 4096;
constexpr uint32_t table_max_pages = 100;

struct Row
{
    uint32_t id;
    std::array<char, COLUMN_USERNAME_SIZE> username;
    std::array<char, COLUMN_EMAIL_SIZE> email;
};

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute);

constexpr uint32_t id_size = size_of_attribute(Row, id);
constexpr uint32_t username_size = size_of_attribute(Row, username);
constexpr uint32_t email_size = size_of_attribute(Row, email);
constexpr uint32_t id_offset = 0;
constexpr uint32_t username_offset = id_offset + id_size;
constexpr uint32_t email_offset = username_offset + username_size;
constexpr uint32_t row_size = username_size + email_size + id_size;
constexpr uint32_t rows_per_page = page_size / row_size;
constexpr uint32_t table_max_rows = rows_per_page * table_max_pages;

class RowManipulator
{
public:
    static void serialize_row(const Row* source, std::byte* destination)
    {
        memcpy(destination + id_offset, &(source->id), id_size);
        memcpy(destination + username_offset, &(source->username), username_size);
        memcpy(destination + email_offset, &(source->email), email_size);
    }
    static void deserialize_row(const std::byte* source, Row* destination)
    {
        memcpy(&(destination->id), source + id_offset, id_size);
        memcpy(&(destination->username), source + username_offset, username_size);
        memcpy(&(destination->email), source + email_offset, email_size);
    }
};

struct Table {
public:
    uint32_t num_rows;
    std::byte* pages[table_max_pages];
};

class TableManipulator
{
public:
    static std::byte* row_slot(Table* table, uint32_t row_num)
    {
        uint32_t page_num = row_num / rows_per_page;
        std::byte* page = table->pages[page_num];
        if (page == nullptr)
        {
            // Allocate memory only when we try to access page 
            // Caution: надо бы потом не забыть память почистить
            page = table->pages[page_num] = new std::byte[page_size];
        }
        uint32_t row_offset = row_num % rows_per_page;
        uint32_t byte_offset = row_offset * row_size;
        return page + byte_offset;
    }
};