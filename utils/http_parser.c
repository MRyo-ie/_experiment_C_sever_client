#include "http_parser.h"

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

// HTTP Request ヘッダーを構造体にパースする。
void parse_HTTP_req(char *http_req_str, http_req parsed_req) {
    /*
        GET / HTTP/1.1
        Host: localhost:10000
        User-Agent: 【】[[curl/7.64.1], [Wget/1.20.3 (darwin19.0.0)]]
        Accept: * / *
    */
    int n = find_header_term(http_req_str);
    printf("[Info] str_len(http_req_str) : %d,    n : %d\n", str_len(http_req_str), n);
}
