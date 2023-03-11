#include "httpconn.hh"
#include <iostream>
#include "Buffer.h"
#include "Connection.hh"
#include "TcpServer.hh"
HTTP::HTTP() = default;
HeaderState HTTP::Prase(const char *string) {  // NOLINT TODO simplfy
  std::string header = string;
  int key_start = -1;
  int key_end = -1;
  int value_start = -1;
  int value_end = -1;
  int now_read_line_begin = 0;
  bool not_finish = true;
  int index = 0;
  for (; index < static_cast<int>(header.size()) && not_finish; ++index) {
    switch (h_state_) {
      case H_START: {
        if (header[index] == '\n' || header[index] == '\r') {
          break;
        }
        h_state_ = H_KEY;
        key_start = index;
        now_read_line_begin = index;
        break;
      }
      case H_KEY: {
        if (header[index] == ':') {
          key_end = index;
          if (key_end - key_start <= 0) {
            return PARSE_HEADER_ERROR;
          }
          h_state_ = H_COLON;
        } else if (header[index] == '\n' || header[index] == '\r') {
          return PARSE_HEADER_ERROR;
        }
        break;
      }
      case H_COLON: {
        if (header[index] == ' ') {
          h_state_ = H_SPACES_AFTER_COLON;
        } else {
          return PARSE_HEADER_ERROR;
        }
        break;
      }
      case H_SPACES_AFTER_COLON: {
        h_state_ = H_VALUE;
        value_start = index;
        break;
      }
      case H_VALUE: {
        if (header[index] == '\r') {
          h_state_ = H_CR;
          value_end = index;
          if (value_end - value_start <= 0) {
            return PARSE_HEADER_ERROR;
          }
        } else if (index - value_start > 255) {
          return PARSE_HEADER_ERROR;
        }
        break;
      }
      case H_CR: {
        if (header[index] == '\n') {
          h_state_ = H_LF;
          std::string key(header.begin() + key_start, header.begin() + key_end);
          std::string value(header.begin() + value_start, header.begin() + value_end);
          headers_[key] = value;
          now_read_line_begin = index;
        } else {
          return PARSE_HEADER_ERROR;
        }
        break;
      }
      case H_LF: {
        if (header[index] == '\r') {
          h_state_ = H_END_CR;
        } else {
          key_start = index;
          h_state_ = H_KEY;
        }
        break;
      }
      case H_END_CR: {
        if (header[index] == '\n') {
          h_state_ = H_END_LF;
        } else {
          return PARSE_HEADER_ERROR;
        }
        break;
      }
      case H_END_LF: {
        not_finish = false;
        key_start = index;
        now_read_line_begin = index;
        break;
      } break;
    }
  }
  if (h_state_ == H_END_LF) {
    header = header.substr(index);
    return PARSE_HEADER_SUCCESS;
  }
  header = header.substr(now_read_line_begin);
  std::cout << "prase" << std::endl;
  return PARSE_HEADER_AGAIN;
}

void HTTP::Response() {
  if (!headers_.empty()) {
    std::cout << "prase success" << std::endl;
  } else {
    std::cout << "prase error" << std::endl;
  }
}

URIState HTTP::ParseUri(const char *string) {
  std::string str = string;
  std::string cop = str;
  // 读到完整的请求行再开始解析请求
  size_t pos = str.find('\r', now_read_pos_);
  if (pos < 0) {
    return PARSE_URI_AGAIN;
  }
  // 去掉请求行所占的空间，节省空间
  std::string request_line = str.substr(0, pos);
  if (str.size() > pos + 1) {
    str = str.substr(pos + 1);
  } else {
    str.clear();
  }
  // Method
  int pos_get = request_line.find("GET");
  int pos_post = request_line.find("POST");
  int pos_head = request_line.find("HEAD");

  if (pos_get >= 0) {
    pos = pos_get;
    method_ = METHOD_GET;
  } else if (pos_post >= 0) {
    pos = pos_post;
    method_ = METHOD_POST;
  } else if (pos_head >= 0) {
    pos = pos_head;
    method_ = METHOD_HEAD;
  } else {
    return PARSE_URI_ERROR;
  }

  // filename
  pos = request_line.find('/', pos);
  if (pos < 0) {
    file_name_ = "index.html";
    http_version_ = HTTP_11;
    return PARSE_URI_SUCCESS;
  } else {
    size_t _pos = request_line.find(' ', pos);
    if (_pos < 0)
      return PARSE_URI_ERROR;
    else {
      if (_pos - pos > 1) {
        file_name_ = request_line.substr(pos + 1, _pos - pos - 1);
        size_t __pos = file_name_.find('?');
        if (__pos >= 0) {
          file_name_ = file_name_.substr(0, __pos);
        }
      }

      else {
        file_name_ = "index.html";
      }
    }
    pos = _pos;
  }
  // cout << "file_name_: " << file_name_ << endl;
  // HTTP 版本号
  pos = request_line.find('/', pos);
  if (pos < 0) {
    return PARSE_URI_ERROR;
  } else {
    if (request_line.size() - pos <= 3) {
      return PARSE_URI_ERROR;
    }
    std::string ver = request_line.substr(pos + 1, 3);
    if (ver == "1.0") {
      http_version_ = HTTP_10;
    } else if (ver == "1.1") {
      http_version_ = HTTP_11;
    } else {
      return PARSE_URI_ERROR;
    }
  }
  return PARSE_URI_SUCCESS;
}