#include <atomic>
#include <cerrno>
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
#include <fcntl.h>
#include <unistd.h>
#include "row.hpp"


constexpr uint32_t page_size = 4096;
constexpr uint32_t table_max_pages = 100;

constexpr uint32_t row_size = COLUMN_USERNAME_SIZE + COLUMN_EMAIL_SIZE + id_size;
constexpr uint32_t rows_per_page = page_size / row_size;
constexpr uint32_t table_max_rows = rows_per_page * table_max_pages;

class Pager
{
public:
    Pager(const std::string& inputFileName, const uint32_t table_pages = table_max_pages): num_rows(0)
    {
        this->filename = inputFileName;
        int file_descriptor = open(inputFileName.c_str(),  O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

        if (file_descriptor == -1)
        {
            std::cout << "can't create file\n";
            exit(EXIT_FAILURE);
        }

        off_t file_length = lseek(file_descriptor, 0, SEEK_END);

        this->file_descriptor = file_descriptor;
        this->file_length = lseek(file_descriptor, 0, SEEK_END);

        this->num_rows = num_rows;
        for (uint32_t i = 0; i < table_pages; i++)
        {
            this->pages[i] = nullptr;
        }
    };

    ~Pager()
    {
        if (this->file_descriptor != -1)
        {
            close(file_descriptor);
        }

        for (auto& page : pages)
        {
            if (page != nullptr)
            {
                delete[] page;
            }
        }
    }

    uint32_t getFileLength()
    {
        return this->file_length;
    }

    void set_page(uint32_t page_ind)
    {
        try {
            pages.at(page_ind) = new std::byte[page_size];
        } 
        catch (std::out_of_range& exception) 
        {
            std::cout << "The page at " << page_ind << " doesn't exist!" << std::endl;
            std::cout << "Pages haven't been set!";
        }
    }

    std::byte* get_page(uint32_t page_ind)
    {
        try 
        {
            if (pages[page_ind] == nullptr)
            {
                pages[page_ind] = new std::byte[page_size];

                uint32_t offset = page_ind * page_size;

                if (file_length >= offset + page_size)
                {
                    lseek(this->file_descriptor, offset, SEEK_SET);
                    ssize_t bytes_read = read(this->file_descriptor, pages[page_ind], page_size);

                    if (bytes_read == -1)
                    {
                        std::cout << "Error reading file: " << errno << std::endl;

                        // TODO: rework not ot exit, but return nullptr and prevent memory leak.
                        exit(EXIT_FAILURE);
                    }
                }
            }

            return pages.at(page_ind);
        } 
        catch (std::out_of_range& exception) 
        {
            std::cout << "The page at " << page_ind << " is out of bounds!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    auto& get_pages() 
    {
        return pages;
    };

private:
    std::string filename;
    int file_descriptor;
    uint32_t file_length;

    uint32_t num_rows;
    std::array<std::byte*, table_max_pages> pages;

};

class Table {
public:
    Table(const std::string filename = "", const uint32_t table_pages = table_max_pages)
    {
        this->pager = new Pager(filename);
        this->num_rows = pager->getFileLength() / row_size;


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