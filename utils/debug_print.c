#include "debug_print.h"

// 前処理（インデント）？

// 後処理（改行）
void debug_print_post_process(bool is_end_line) {
    if (is_end_line) printf("\n");
}

// .h
void debug_print(const char* mode_str, const char* message, bool is_end_line, bool is_debug) {
    if (is_debug_mode_global  && is_debug) {
        printf("[%s] %s", mode_str, message);
        debug_print_post_process(is_end_line);
    }
}
void debug_print_msg(const char* message, bool is_end_line, bool is_debug) {
    if (is_debug_mode_global  && is_debug) {
        printf("%s", message);
        debug_print_post_process(is_end_line);
    }
}

void debug_print_char(const char* var_name, char var, bool is_end_line, bool is_debug) {
    if (is_debug_mode_global  && is_debug) {
        printf("    %s : '%c'", var_name, var);
        debug_print_post_process(is_end_line);
    }
}
void debug_print_str(const char* var_name, const char* var, bool is_end_line, bool is_debug) {
    if (is_debug_mode_global  && is_debug) {
        printf("    %s : \"%s\"", var_name, var);
        debug_print_post_process(is_end_line);
    }
}
void debug_print_int(const char* var_name, int var, bool is_end_line, bool is_debug) {
    if (is_debug_mode_global  && is_debug) {
        printf("    %s : %d", var_name, var);
        debug_print_post_process(is_end_line);
    }
}
void debug_print_float(const char* var_name, float var, bool is_end_line, bool is_debug) {
    if (is_debug_mode_global  && is_debug) {
        printf("    %s : %f", var_name, var);
        debug_print_post_process(is_end_line);
    }
}
