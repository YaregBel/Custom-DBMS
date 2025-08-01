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

constexpr uint32_t COLUMN_USERNAME_SIZE = 32;
constexpr uint32_t COLUMN_EMAIL_SIZE = 256;
constexpr uint32_t page_size = 4096;
constexpr uint32_t table_max_pages = 100;

class Row
{
public:

    Row() = default;

    Row(uint32_t id, const std::array<char, COLUMN_USERNAME_SIZE> username, const std::array<char, COLUMN_EMAIL_SIZE> email)
        : id(id), username(username), email(email) {}

    Row(uint32_t id, const std::string& username, const std::string& email)
        : id(id)
    {
        setUsernameFromString(username);
        setEmailFromString(email);
    }

    Row(const Row& other)
        : id(other.id),
          username(other.username),
          email(other.email)
    {}

    Row& operator=(const Row& other)
    {
        if (this != &other)
        {
            id = other.id;
            username = other.username;
            email = other.email;
        }
        return *this;
    }

    ~Row() = default;

    void setStatementId(const std::string idStr)
    {
        this->id = std::stoi(idStr);
    }

    void setUsernameFromArray(const std::array<char, COLUMN_USERNAME_SIZE> username)
    {
        this->username = username;
        this->username[COLUMN_USERNAME_SIZE - 1] = '\0';
    }

    void setEmailFromArray(const std::array<char, COLUMN_EMAIL_SIZE> email)
    {
         this->email = email;
         this->email[COLUMN_EMAIL_SIZE - 1] = '\0';
    }

    void setUsernameFromString(const std::string& value)
    {
        strncpy(username.data(), value.c_str(), COLUMN_USERNAME_SIZE - 1);
        username[COLUMN_USERNAME_SIZE - 1] = '\0';
    }

    void setEmailFromString(const std::string& value)
    {
        strncpy(email.data(), value.c_str(), COLUMN_EMAIL_SIZE - 1);
        email[COLUMN_EMAIL_SIZE - 1] = '\0';
    }

    bool operator==(const Row& other) const
    {
        return id == other.id &&
               std::equal(username.begin(), username.end(), other.username.begin()) &&
               std::equal(email.begin(), email.end(), other.email.begin());
    }

    bool operator!=(const Row& other) const
    {
        return !(*this == other);
    }

    uint32_t id;
    std::array<char, COLUMN_USERNAME_SIZE> username;
    std::array<char, COLUMN_EMAIL_SIZE> email;

private:

};

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute);

constexpr uint32_t id_size = size_of_attribute(Row, id);
constexpr uint32_t id_offset = 0;
constexpr uint32_t username_offset = id_offset + id_size;
constexpr uint32_t email_offset = username_offset + COLUMN_USERNAME_SIZE;
constexpr uint32_t row_size = COLUMN_USERNAME_SIZE + COLUMN_EMAIL_SIZE + id_size;
constexpr uint32_t rows_per_page = page_size / row_size;
constexpr uint32_t table_max_rows = rows_per_page * table_max_pages;

class RowManipulator
{
public:
    static void serialize_row(const Row* source, std::byte* destination)
    {
        memcpy(destination + id_offset, &(source->id), id_size);
        memcpy(destination + username_offset, source->username.data(), COLUMN_USERNAME_SIZE);
        memcpy(destination + email_offset, source->email.data(), COLUMN_EMAIL_SIZE);
    }
    static void deserialize_row(const std::byte* source, Row* destination)
    {
        memcpy(&(destination->id), source + id_offset, id_size);
        memcpy(destination->username.data(), source + username_offset, COLUMN_USERNAME_SIZE);
        memcpy(destination->email.data(), source + email_offset, COLUMN_EMAIL_SIZE);
    }

    static void print_row(Row* row)
    {
        std::cout << "(" << row->id << ", " << row->username.data() << ", " << 
            row->email.data() << ")" << std::endl;
    }

    // template <size_t N>
    // static void print_arr(const std::array<char, N>& arr) {
    //     for (char c : arr) {
    //         if (c == '\0') break;
    //         std::cout << c;
    //     }
    // }
};

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