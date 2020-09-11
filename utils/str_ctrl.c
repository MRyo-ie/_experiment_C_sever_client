#include "str_ctrl.h"

// 文字列(char*) の文字数を数える。
int str_len(const char *s) {
    int n = 0;
    while (*s++) n++;
    return n;
}

