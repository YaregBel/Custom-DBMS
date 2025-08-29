#include "table.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>

class DB_Handler
{
public:
    static Table* db_open(const std::string& filename)
    {
        Table* table = new Table(filename);

        return table;
    }

    static void db_close(Table* table)
    {
        Pager* pager = table->pager;
        uint32_t num_full_pages = table->num_rows / rows_per_page;

        for (uint32_t page_ind = 0; page_ind < table_max_pages; page_ind++)
        {
            if (pager->get_page(page_ind) == nullptr)
            {
                continue;
            }

            pager_flush(pager, page_ind, page_size);
            delete[] pager->get_page(page_ind);
            pager->set_page(page_ind, nullptr);
        }

        // Это на случай, если остались не удаленные строки в конце файл.
        // Когда будет осуществлен переход к концепции B-tree, данный блок кода не потребуется.
        uint32_t num_additional_rows = table->num_rows % rows_per_page;
        if (num_additional_rows > 0)
        {
            uint32_t page_num = num_full_pages;
            if (pager->get_page(page_num) != nullptr)
            {
                pager_flush(pager, page_num, num_additional_rows * row_size);
                delete[] pager->get_page(page_num);
                pager->set_page(page_num, nullptr);
            }
        }

        int result = close(pager->get_file_descriptor());
        if (result == -1)
        {
            std::cout << "Error closing db file.\n" << std::endl;
            exit(EXIT_FAILURE);
        }
        for (uint32_t page_ind = 0; page_ind < table_max_pages; page_ind++)
        {
            std::byte* page = pager->get_page(page_ind);
            if (page)
            {
                delete[] page;
                pager->set_page(page_ind, nullptr);
            }
        }

        delete(pager);
        delete(table);

    }

    static void pager_flush(Pager* pager, uint32_t page_ind, uint32_t size)
    {
        if (pager->get_page(page_ind) == nullptr)
        {
            std::cout << "Tried to flush null page\n";
            exit(EXIT_FAILURE);
        }

        off_t offset = lseek(pager->get_file_descriptor(), page_ind * page_size, SEEK_SET);

        if (offset == -1)
        {
            std::cout << "Error seeking: " << errno << std::endl; 
            exit(EXIT_FAILURE);
        }

        // Returns: 4096
        ssize_t bytes_written = write(pager->get_file_descriptor(), pager->get_page(page_ind), size);
        
        if (bytes_written == -1)
        {
            std::cout << "Error writing: " << errno;
            exit(EXIT_FAILURE);
        }
    }
};