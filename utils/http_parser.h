#include <stdio.h>
#include <string.h>

#include "debug_print.h"

#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H
// typedef struct http_req_template {
//     char *method;
//     char *path;
//     char *http_ver;
//     char *host;
//     char *user_agent;
//     char *accept;
// } http_req;
typedef struct http_req_template {
    char method[8];
    char path[256];
    char http_ver[16];
    char host[256];
    char user_agent[256];
    char accept[128];
} http_req;

typedef enum {
    Host,
    User_Agent,
    Accept,
    HTTP_HEAD_KEY_NUM
} HTTP_HEAD_KEYS;

// key の検索用
static char http_head_keys[HTTP_HEAD_KEY_NUM][20] = {
    "Host", 
    "User-Agent", 
    "Accept",
};
#endif

// リクエストヘッダの末尾探索
int find_header_term(char *line);

// HTTP Request ヘッダーを構造体にパースする。
void parse_HTTP_req(char *http_req_str, http_req* parsed_req);

void print_parsed_req(http_req *parsed_req);
