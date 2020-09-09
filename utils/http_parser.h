#include <stdio.h>

#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H
typedef struct http_req_template {
    char *method;
    float http_ver;
    char *host;
    char *accept;
    char *user_agent;
} http_req;
#endif

// リクエストヘッダの末尾探索
int find_header_term(char *line);

// HTTP Request ヘッダーを構造体にパースする。
void parse_HTTP_req(char *http_req_str, http_req parsed_req);
