#pragma once
#include <cstdio>
#include <cstdlib>

namespace util {
  void errif(bool condition, const char *errmsg){
    if(condition){
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}
};
