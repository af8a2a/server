#pragma once
#include <cstdio>
#include <cstdlib>

namespace util {
  void static errif(bool condition, const char *errmsg){
    if(condition){
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}
};
