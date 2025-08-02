#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <array>
#include <sys/types.h>
#include <array>
#include <cstring>
#include <algorithm>
#include "row.hpp"

constexpr uint32_t page_size = 4096;
constexpr uint32_t table_max_pages = 100;

constexpr uint32_t row_size = COLUMN_USERNAME_SIZE + COLUMN_EMAIL_SIZE + id_size;
constexpr uint32_t rows_per_page = page_size / row_size;
constexpr uint32_t table_max_rows = rows_per_page * table_max_pages;

class Table {
public:
    Table(const uint32_t table_pages = table_max_pages)
    {
        this->num_rows = 0;
        for (int i = 0; i < table_pages; i++)
        {
            this->pages[i] = NULL;
        }
    }
    ~Table()
    {
        for (int i = 0; table_max_pages; i++)
        {
            if ((pages[i]) == nullptr)
            {
                break;
            }   
            else 
            {
                delete[] pages[i];
            }
        }
    }

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