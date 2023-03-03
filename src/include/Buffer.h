/**
 * @file Buffer.h
 * @author 冯岳松 (yuesong-feng@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-01-04
 *
 * @copyright Copyright (冯岳松) 2022
 *
 */
#pragma once
#include <string>
#include "Macros.h"

class Buffer {
 public:
  DISALLOW_COPY_AND_MOVE(Buffer);
  Buffer() = default;
  ~Buffer() = default;

  void Append(const char *_str, int _size);
  ssize_t Size();
  const char *ToStr();
  void Clear();
  void Getline();
  void SetBuf(const char *buf);

 private:
  std::string buf_;
};
