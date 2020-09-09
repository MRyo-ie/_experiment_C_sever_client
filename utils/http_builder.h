#include <stdio.h>
#include <unistd.h>
#include "http_parser.h"

typedef struct http_res_template {
    char *method;
    char sex;
    int age;
    char *add;
    char *job;
} http_res;

void build_HTTP_res(http_req parsed_req);
