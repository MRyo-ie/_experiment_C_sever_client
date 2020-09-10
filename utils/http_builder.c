#include "http_builder.h"

static bool is_debug_mode = true;

// int build_HTML_header(char *res_header) {
//     sprintf(res_header, "");
//     return 0;
// }

void print_struct_res(const char* place, http_res *build_res) {
    is_debug_mode = true;
    debug_print("Info", place, false, is_debug_mode);
    debug_print_msg("  build_res の中身を print します。", true, is_debug_mode);
    debug_print_str("http_ver", build_res->http_ver, true, is_debug_mode);
    debug_print_int("code    ", build_res->code, true, is_debug_mode);
    debug_print_str("message ", build_res->message, true, is_debug_mode);
    debug_print_str("content_type", build_res->content_type, true, is_debug_mode);
    is_debug_mode = false;
}

void build_HTTP_res(http_req* parsed_req, http_res* build_res) {
    print_struct_req("http_builder.c", parsed_req);

    // Response用のデータを集める。（ = 構造体を埋める）
    sprintf(build_res->http_ver, "%s", parsed_req->http_ver);
    build_res->code = 200;  // OK
    sprintf(build_res->message, "%s", "OK");
    sprintf(build_res->content_type, "%s", "text/html");
    // ファイル読み込み
    int fsize = read_file_binary("test.txt", build_res->body);
    debug_print("Info", "ファイルの中身を表示テスト", false, is_debug_mode);
    debug_print_int("fsize", fsize, true, is_debug_mode);
    debug_print_msg(build_res->body, true, is_debug_mode);


    print_struct_res("http_builder.c", build_res);
}
