#include "http_parser.h"

static bool is_debug_mode = false;

// 文字列(char*) の文字数を数える。
int str_len(const char *s) {
    int n = 0;
    while (*s++) n++;
    return n;
}

// リクエストヘッダの末尾探索
int find_header_term(char *line) {
    register char *p, *q;
    register int c;
    c = 0;
    p = line;
    q = NULL;
    while (*p && c < str_len(line)) {
        if (*p == '\n') {
            if (*(p + 1) == '\n') { /* LF LF */
                p += 2;
                goto find;
            }
            if (*(p + 1) == '\r') { /* LF CR (LF) */
                p += 3;
                goto find;
            }
        }
    next:
        p++;
        c++;
    }
    goto fail;
find:
    return p - line;
fail:
    return -1;
}




// HTTP Request ヘッダ ー（１行分）から、メソッド、HTTPバージョン、key:value
// を抽出する。
int extract_HTTP_header(char *line, http_req* parsed_req) {
    // 「メソッド + HTTPバージョン」「key: value」 どちらなのかを判定する。
    char *p = line,
         // 「key: value」の区切り文字があるかどうかで判定。
         //     なければ null = 「メソッド + HTTPバージョン」
        *div_p = strstr(line, ": "), *buf_p;
    int div_num = 0;

    // 判定結果にしたがって、parsed_req に情報を格納する。
    if (div_p == NULL) {
        // ＜1行目＞ GET / HTTP/1.1  →  {"GET", "/", "HTTP/1.1"}
        char buf_arr[3][256];
        buf_p = &(buf_arr[0][0]);  // ポインタは stream !!
        while (true) {
            *buf_p = *p;
            // debug
            debug_print_char("*p", *p, false, is_debug_mode);
            debug_print_char("*buf_p", *buf_p, false, is_debug_mode);
            debug_print_int("div_num", div_num, true, is_debug_mode);
            // 区切り文字： ' '
            if (*p == ' ' || *p == '\0') {
                *buf_p = '\0';  // * をつけると中身の参照になる。
                // printf("  buf_arr[%d] : \"%s\"\n", div_num, buf_arr[div_num]);
                // スペース区切り
                switch (div_num) {
                    case 0:  //  GET, POST, HEAD
                        sprintf(parsed_req->method, "%s", buf_arr[div_num]);
                        debug_print_str("    parsed_req->method",
                                        parsed_req->method, true, is_debug_mode);
                        break;
                    case 1:  //  /path
                        sprintf(parsed_req->path, "%s", buf_arr[div_num]);
                        debug_print_str("    parsed_req->path",
                                        parsed_req->path, true, is_debug_mode);
                        break;
                    case 2:  //  HTTP/1.1
                        sprintf(parsed_req->http_ver, "%s", buf_arr[div_num]);
                        debug_print_str("    parsed_req->http_ver",
                                        parsed_req->http_ver, true, is_debug_mode);
                        break;
                    default:
                        debug_print("Error", "div_num の値が不正です。", true, is_debug_mode);
                        break;
                }
                // 行の最後なら、break する。
                if (*p == '\0') break;
                // buf_p ポインタの参照先を次の配列に差し替える。
                div_num++;
                buf_p = &(buf_arr[div_num][0]);
            } else {
                buf_p++;
            }
            p++;
        }
    } else {  // ＜2行目以降＞ key: val  →  {"key", "val"}
        char key[32], val[256];
        buf_p = &(key[0]);
        HTTP_HEAD_KEYS key_enum = HTTP_HEAD_KEY_NUM;
        while (true) {  // *p == '\0' で抜ける
            *buf_p = *p;
            // debug
            debug_print_char("*p", *p, false, is_debug_mode);
            debug_print_char("*buf_p", *buf_p, false, is_debug_mode);
            debug_print_int("div_num", div_num, true, is_debug_mode);
            // 行の最後なら、break する。
            if (*p == '\0') {
                debug_print_str("val", val, true, is_debug_mode);
                break;
            }
            if ((div_num == 0 && *p == *div_p) || *p == '\0') {
                // ": " を検出した。 => keyを抽出する。
                *buf_p = '\0';
                debug_print_str("key", key, true, is_debug_mode);
                debug_print_msg("      enum 検索中...", true, is_debug_mode);
                for (int i = 0; i < HTTP_HEAD_KEY_NUM; i++) {
                    // 同じなら、その変数の
                    // printf("        key : \"%s\"  =?  \"%s\"", key, http_head_keys[i]);
                    debug_print_int("=>  strcmp(key, http_head_keys[i])",
                                    strcmp(key, http_head_keys[i]), true, is_debug_mode);
                    if (strcmp(key, http_head_keys[i]) == 0) {
                        debug_print_msg("      enum ヒット！", true, is_debug_mode);
                        key_enum = (HTTP_HEAD_KEYS)i;
                        break;
                    }
                }
                // ": " の２文字目（半角スペース）を飛ばす。
                p++;
                // val の取得モードに切り替える。
                div_num = 1;
                buf_p = &(val[0]);
            } else {
                buf_p++;
            }
            p++;
        }
        // key を元に、http_head_keysのメンバ変数を選択 → ポインタを保存する。
        if (key_enum < HTTP_HEAD_KEY_NUM)
            switch (key_enum) {
                case Host:
                    sprintf(parsed_req->host, "%s", val);
                    break;
                case User_Agent:
                    sprintf(parsed_req->user_agent, "%s", val);
                    break;
                case Accept:
                    sprintf(parsed_req->accept, "%s", val);
                    break;
                default:
                    break;
            }
    }
    return 0;
}

// HTTP Request ヘッダーを構造体にパースする。
void parse_HTTP_req(char *http_req_str, http_req* parsed_req) {
    /*
        GET / HTTP/1.1
        Host: localhost:10000
        User-Agent: 【】[[curl/7.64.1], [Wget/1.20.3 (darwin19.0.0)]]
        Accept: * / *
    */
    int n = find_header_term(http_req_str);
    // debug_print("Info", "HTTPヘッダの切り出しテスト", true, is_debug_mode);
    // debug_print_int("str_len(http_req_str)", str_len(http_req_str), true, is_debug_mode);
    // debug_print_int("n", n, true, is_debug_mode);

    // 行に分解して、行ごとにヘッダを抽出する。
    char line[BUFSIZ];
    int idx = 0;
    char *p = http_req_str;
    debug_print("Info", "行ごとに切り分けられているかテスト", true, is_debug_mode);
    while (*p) {
        line[idx] = *p;
        // 行終わり
        if (*p == '\n' || *p == '\r') {
            line[idx] = '\0';
            if (*p == '\n') {
                debug_print("Info", "", false, is_debug_mode);
                debug_print_str("line", line, true, is_debug_mode);
                if (idx != 0) { // 空行は飛ばす。
                    // 今回の行のヘッダ情報を抽出する。
                    extract_HTTP_header(line, parsed_req);
                    debug_print_str("  [parse_HTTP_req()]  method", parsed_req->method, true, is_debug_mode);
                    debug_print_str("  [parse_HTTP_req()]  path", parsed_req->path, true, is_debug_mode);
                    debug_print_str("  [parse_HTTP_req()]  http_ver", parsed_req->http_ver, true, is_debug_mode);
                    debug_print_msg("", true, is_debug_mode);
                }
                idx = 0;
            }
        } else {
            idx++;
        }
        p++;
    }
    // print_struct_req("http_parser.c", parsed_req);
}



void print_struct_req(const char* place, http_req *parsed_req) {
    is_debug_mode = true;
    debug_print("Info", place, false, is_debug_mode);
    debug_print_msg("  parsed_req の中身を print します。", true, is_debug_mode);
    debug_print_str("method   ", parsed_req->method, true, is_debug_mode);
    debug_print_str("path     ", parsed_req->path, true, is_debug_mode);
    debug_print_str("http_ver ", parsed_req->http_ver, true, is_debug_mode);
    debug_print_str("host     ", parsed_req->host, true, is_debug_mode);
    debug_print_str("accept   ", parsed_req->accept, true, is_debug_mode);
    debug_print_str("user_agent", parsed_req->user_agent, true, is_debug_mode);
    is_debug_mode = false;
}

