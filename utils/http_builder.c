#include "http_builder.h"

static bool is_debug_mode = true;


// 404 not Found! に Response を更新する。
void set_failure_res(http_res* res, int code, const char* message) {
    res->code = code;
    sprintf(res->message, "%s", message);
}

// file IO周りのラッパー
void get_file_list(const char* path, char f_list[2048]) {
    DIR* dirp = opendir(path);
    struct dirent* dp;
    char buf[256];

    if (dirp == NULL) {
        perror("[Error] opendir");
        exit(1);
    }
    strcpy(f_list, "\n【 ファイル一覧 】\n\n");
    while ((dp = readdir(dirp)) != NULL) {
        sprintf(buf, "    %s\n", dp->d_name);
        strcat(f_list, buf);
    }
    closedir(dirp);
}

void read_text_data(http_res* res, const char* fpath) {
    //  http://localhost:10000/test.text
    res->content_length = read_file_binary(fpath, res->body);
    if (res->content_length == -1)
        set_failure_res(res, 404, "Not Found !");
}
void read_img_data(http_res* res, const char* fpath) {
    //  http://localhost:10000/explosionAA.png
    res->content_length = read_file_binary(fpath, res->body);
    if (res->content_length == -1)
        set_failure_res(res, 404, "Not Found !");
}


// path から拡張子を検索して、データの種類を推測する。→ res->content_typez
int extract_ext(const char* path, http_res* res) {
    char *p, *p_e;
    p = strrchr(path, '.');
    p_e = strrchr(path, '\0');
    debug_print("Info", "ファイル拡張子情報", true, is_debug_mode);
    debug_print_int("'.'の位置", p - path, false, is_debug_mode);
    debug_print_int("'\\0'の位置", p_e - path, false, is_debug_mode);
    debug_print_int("拡張子の文字数", (p_e - p - 1), true, is_debug_mode);
    if (p != NULL || (p_e - p) < 5) {
        char ext[5];
        sprintf(ext, "%s", (p + 1));
        for (int t = 0; t < EXTS_TYPES_NUM; t++) {
            for (int i = 0; i < exts_datas[t].ext_num; i++) {
                debug_print_str("  ext", ext, false, is_debug_mode);
                debug_print_str("ext_names[i]", exts_datas[t].ext_names[i], false, is_debug_mode);
                debug_print_int("=>  strcmp(ext, ext_names[i])", strcmp(ext, exts_datas[t].ext_names[i]), true, is_debug_mode);

                if (strcmp(ext, exts_datas[t].ext_names[i]) == 0) {
                    debug_print_msg("    enum ヒット！", true, is_debug_mode);
                    // key_enum = (TEXT_EXTENSIONS)i;
                    switch (t) {
                        case text:
                            switch (i) {
                                case txt:
                                    sprintf(res->content_type, "%s", "text/plain; charset=UTF-8");
                                    break;
                                case html:
                                    sprintf(res->content_type, "%s", "text/html; charset=UTF-8");
                                    break;
                                default:
                                    break;
                            }
                            read_text_data(res, path);  // ファイル読み込み
                            return 0;
                        case binary:
                            sprintf(res->content_type, "image/%s", ext);
                            read_img_data(res, path);  // ファイル読み込み
                            return 0;
                        default:
                            break;
                    }
                }
            }
        }
    }
    // 拡張子がない？
    set_failure_res(res, 404, "Not Found !");
    return -1;
}

// reqを解析 → resを埋める。
int analyze_path(http_req* req, http_res* res) {
    print_struct_req("http_builder.c", req);
    // res を初期化する。
    // sprintf(res->http_ver, "%s", req->http_ver);
    sprintf(res->http_ver, "%s", "HTTP/1.0");
    res->code = 200;  // OK
    sprintf(res->message, "%s", "OK");

    // data/ をルートとして、req->path の相対パスを作る。
    char path[128];
    sprintf(path, "datas%s", req->path);
    /*--  パスが安全か確認する。  --*/
    ////  1. "/../" が含まれていたら、403 Forbidden を返す。
    if (strstr(path, "/../") != NULL) {
        set_failure_res(res, 403, "Forbidden");
        return 403;
    }

    // Request が file か dir かを確認する。
    char *p_end = strrchr(path, '\0');
    if (*(p_end-1) == '/') {
        /*--  dir  --*/
        char f_list[2048];
        get_file_list(path, f_list);
        // res を更新する。
        strcpy(res->body, f_list);
        sprintf(res->content_type, "%s", "text/plain; charset=UTF-8");
        res->content_length = str_len(f_list);
    } else {
        /*--  file  --*/
        // path から拡張子を検索して、データの種類を推測する。→ res->content_type
        extract_ext(path, res);
    }
    return res->code;
}



// HTTP Response を build する。
void build_HTTP_res(http_res* res, char* buf) {
    /*
        HTTP/1.1 200 OK
        Content-Type: text/html
        Content-Length: 6145
        // 後回し
        Date: Wed, 09 Sep 2020 06:11:31 GMT
        Last-Modified: Fri, 28 Aug 2020 01:34:20 GMT
        Accept-Ranges: bytes
    */
    // HTTP Response を build する。
    char line_buf[BUFSIZ * 100];
    buf[0] = '\0';
    // HTTP/1.1 200 OK
    sprintf(line_buf, "%s %d %s\r\n", res->http_ver, res->code, res->message);
    strcat(buf, line_buf);
    // Content-Type: text/html
    sprintf(line_buf, "Content-Type: %s\r\n", res->content_type);
    strcat(buf, line_buf);
    // Content-Length: 【データのサイズ】
    sprintf(line_buf, "Content-Length: %d\r\n", res->content_length);
    strcat(buf, line_buf);
    // Header 終わり
    strcat(buf, "\r\n");
    // body
    strcat(buf, res->body);
    // char 終了
    // strcat(buf, "\0");

    // debug_print("Info", "HTTP Response ↓", true, is_debug_mode);
    // debug_print_msg(buf, true, is_debug_mode);
}

// debug
void print_struct_res(const char* place, http_res* res, bool is_print_body) {
    bool tmp = is_debug_mode;
    is_debug_mode = true;
    debug_print("Info", place, false, is_debug_mode);
    debug_print_msg("  res の中身を print します。", true, is_debug_mode);
    debug_print_str("http_ver    ", res->http_ver, true, is_debug_mode);
    // debug_print_char("http_ver[0]", res->http_ver[0], true, is_debug_mode);
    debug_print_int("code        ", res->code, true, is_debug_mode);
    debug_print_str("message     ", res->message, true, is_debug_mode);
    debug_print_str("content_type", res->content_type, true, is_debug_mode);
    debug_print_int("content_length", res->content_length, true, is_debug_mode);
    if (is_print_body) {
        debug_print_str("body        ", "↓", true, is_debug_mode);
        debug_print_msg(res->body, true, is_debug_mode);
    }
    is_debug_mode = tmp;
}
