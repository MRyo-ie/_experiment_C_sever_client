#include "http_builder.h"

static const bool is_debug_mode = true;

// int build_HTML_header(char *res_header) {
//     sprintf(res_header, "");
//     return 0;
// }


void build_HTTP_res(http_req* parsed_req) {
    print_parsed_req("http_builder.c", parsed_req);
}



