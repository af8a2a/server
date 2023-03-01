#pragma once
#include <string>

class Buffer
{
private:
    std::string buf;
public:
    Buffer();
    ~Buffer();
    void append(const char* _str, int _size);
    auto size() -> ssize_t;
    auto c_str() -> const char*;
    void clear();
    void getline();
};

