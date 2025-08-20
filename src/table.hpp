#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <array>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <array>
#include <cstring>
#include <algorithm>
#include <filesystem>
#include "row.hpp"


constexpr uint32_t page_size = 4096;
constexpr uint32_t table_max_pages = 100;

constexpr uint32_t row_size = COLUMN_USERNAME_SIZE + COLUMN_EMAIL_SIZE + id_size;
constexpr uint32_t rows_per_page = page_size / row_size;
constexpr uint32_t table_max_rows = rows_per_page * table_max_pages;

class Pager
{
public:
    Pager(const std::string& inputFileName, const uint32_t table_pages = table_max_pages)
    {
        this->filename = inputFileName;

        std::fstream file(inputFileName);

        if (file.is_open())
        {
            this->fileLength = std::filesystem::file_size(inputFileName);

            this->num_rows = num_rows;
            for (int i = 0; i < table_pages; i++)
            {
                this->pages[i] = NULL;
            }

        }
        else 
        {
            std::cout << "Unable to open file\n";
            exit(EXIT_FAILURE);
        }
    };

    ~Pager() = default;

    uint32_t getFileLength()
    {
        return this->fileLength;
    }

    void set_page(uint32_t pageInd)
    {
        try {
            pages.at(pageInd) = new std::byte[page_size];
        } 
        catch (std::out_of_range& exception) 
        {
            std::cout << "The page at " << pageInd << " doesn't exist!" << std::endl;
            std::cout << "Pages haven't been set!";
        }
    }

    std::byte* get_page(uint32_t pageInd)
    {
        try 
        {
            return pages.at(pageInd);
        } 
        catch (std::out_of_range& exception) 
        {
            std::cout << "The page at " << pageInd << " doesn't exist!" << std::endl;
            std::cout << "Return first page instead!";
            return pages[pageInd];
        }
    }

    auto& get_pages() 
    {
        return pages;
    };

private:
    std::string filename;
    int fileDescriptor;
    uint32_t fileLength;

    uint32_t num_rows;
    std::array<std::byte*, table_max_pages> pages;

};

class Table {
public:
    Table(const uint32_t table_pages = table_max_pages, const std::string filename = "")
    {
        Pager* pager = new Pager(filename);
        uint32_t num_rows = pager->getFileLength() / row_size;


        // this->num_rows = num_rows;
        // for (int i = 0; i < table_pages; i++)
        // {
        //     this->pages[i] = NULL;
        // }
    }
    // ~Table()
    // {
    //     for (int i = 0; table_max_pages; i++)
    //     {
    //         if ((pages[i]) == nullptr)
    //         {
    //             break;
    //         }   
    //         else 
    //         {
    //             delete[] pages[i];
    //         }
    //     }
    // }

    uint32_t num_rows;
    //std::byte* pages[table_max_pages];
    Pager* pager;
};

class TableManipulator
{
public:
    static std::byte* row_slot(Table* table, const uint32_t row_num)
    {
        uint32_t page_num = row_num / rows_per_page;
        std::byte* page = table->pager->get_page(page_num);
        if (page == nullptr)
        {
            // Allocate memory only when we try to access page 
            // Caution: надо бы потом не забыть память почистить

            table->pager->set_page(page_num);
            page = table->pager->get_page(page_num);
        }
        uint32_t row_offset = row_num % rows_per_page;
        uint32_t byte_offset = row_offset * row_size;
        return page + byte_offset;
    }
};