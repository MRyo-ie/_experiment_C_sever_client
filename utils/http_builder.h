#include <dirent.h>
#include <stdio.h>
#include <string.h>
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

typedef enum {
    txt,
    html,
    TEXT_EXTS_NUM
} TEXT_EXTENSIONS;
typedef enum {
    jpg,
    png,
    bmp,
    IMG_EXTS_NUM
} IMG_EXTENSIONS;
typedef enum {
    text,
    binary,
    EXTS_TYPES_NUM
} EXTS_TYPES;

// key の検索用
struct data_type {
    EXTS_TYPES ext_type;
    int ext_num;
    char *exts_names[5];
};
struct data_type data_types[EXTS_TYPES_NUM] = {
    { text, TEXT_EXTS_NUM, { "txt", "html" } },
    { binary, IMG_EXTS_NUM, { "jpg",  "png",  "bmp" } }
};
#endif


int analyze_path(http_req* req, http_res* res);
void build_HTTP_res(http_req* parsed_req, http_res* build_res, char* buf);

void print_struct_res(const char* place, http_res* parsed_res, bool is_print_body);
