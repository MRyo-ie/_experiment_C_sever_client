#include "http_builder.h"

static bool is_debug_mode = true;

// int build_HTML_header(char *res_header) {
//     sprintf(res_header, "");
//     return 0;
// }

void print_struct_res(const char* place, http_res* res, bool is_print_body) {
    bool tmp = is_debug_mode;
    is_debug_mode = true;
    debug_print("Info", place, false, is_debug_mode);
    debug_print_msg("  res の中身を print します。", true, is_debug_mode);
    debug_print_str("http_ver ", res->http_ver, true, is_debug_mode);
    debug_print_char("http_ver[0]", res->http_ver[0], true, is_debug_mode);
    debug_print_int("code     ", res->code, true, is_debug_mode);
    debug_print_str("message  ", res->message, true, is_debug_mode);
    debug_print_str("content_type", res->content_type, true, is_debug_mode);
    if (is_print_body) {
        debug_print_int("data_size", res->data_size, true, is_debug_mode);
        debug_print_str("body     ", "↓", true, is_debug_mode);
        debug_print_msg(res->body, true, is_debug_mode);
    }
    is_debug_mode = tmp;
}

void build_HTTP_res(http_req* req, http_res* res, char* buf) {
    print_struct_req("http_builder.c", req);

    // Response用のデータを集める。（ = 構造体を埋める）
    sprintf(res->http_ver, "%s", req->http_ver);
    res->code = 200;  // OK
    sprintf(res->message, "%s", "OK");
    sprintf(res->content_type, "%s", "text/html");
    // ファイル読み込み
    res->data_size = read_file_binary("test.txt", res->body);
    // debug
    print_struct_res("http_builder.c", res, true);

    // HTTP Response を build する。
    char line_buf[BUFSIZ];
    buf[0] = '\0';
    // HTTP/1.1 200 OK
    sprintf(line_buf, "%s %d %s\r\n", res->http_ver, res->code, res->message);
    strcat(buf, line_buf);
    // Content-Type: text/html
    sprintf(line_buf, "Content-Type: %s\r\n", res->content_type);
    strcat(buf, line_buf);
    // Header 終わり
    strcat(buf, "\r\n");
    // body
    strcat(buf, res->body);
    // char 終了
    strcat(buf, "\0");

    debug_print("Info", "HTTP Response ↓", true, is_debug_mode);
    debug_print_msg(buf, true, is_debug_mode);
}
