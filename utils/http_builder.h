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
    char body[BUFSIZ];
} http_res;
#endif

// sprintf() に使うためのテンプレ
static const char __res_tmpl_basis__[] = "%s %d %s";  // HTTP/1.1 200 OK
static const char __res_tmpl_content_type__[] = "Content-Type: %s";  // Content-Type: text/html


void build_HTTP_res(http_req* parsed_req, http_res* build_res);

void print_struct_res(const char* place, http_res* parsed_res);
