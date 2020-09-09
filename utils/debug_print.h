#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// 【参考】変数名を文字に変換して取り出すには？： https://okwave.jp/qa/q299912.html
#define GET_NAME(n, sz) strcpy(sz, #n)

// [Info] を出力するやつ。
void debug_print(char *mode_str, char *message, bool is_end_line);

// var_name : GET_NAME(var, var_name);
void debug_print_str(char *var_name, char *var, bool is_end_line);
void debug_print_int(char *var_name, int var, bool is_end_line);
void debug_print_float(char *var_name, float var, bool is_end_line);
