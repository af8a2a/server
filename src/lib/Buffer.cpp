#include "Buffer.h"
#include <cstring>
#include <iostream>
Buffer::Buffer() = default;

Buffer::~Buffer() = default;

void Buffer::append(const char *_str, int _size) {
  for (int i = 0; i < _size; ++i) {
    if (_str[i] == '\0') break;
    buf.push_back(_str[i]);
  }
}

auto Buffer::size() -> ssize_t { return buf.size(); }

auto Buffer::c_str() -> const char * { return buf.c_str(); }

void Buffer::clear() { buf.clear(); }

void Buffer::getline() {
  buf.clear();
  std::getline(std::cin, buf);
}