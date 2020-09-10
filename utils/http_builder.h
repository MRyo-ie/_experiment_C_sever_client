#include <stdio.h>
#include <unistd.h>
#include "http_parser.h"
#include "file_rw.h"

#ifndef HTTP_BUILDER_H
#define HTTP_BUILDER_H
typedef struct http_res_template {
    char http_ver[16];
    int code;  // 200, 404, 405, ...
    char message[16];
    char content_type[128];  // text/html
    int content_length;
    char body[BUFSIZ*100];
} http_res;
#endif

void analyze_path(http_req* req);
void build_HTTP_res(http_req* parsed_req, http_res* build_res, char* buf);

void print_struct_res(const char* place, http_res* parsed_res, bool is_print_body);
