#include "file_rw.h"

//ファイルサイズを取得する
//戻り値が-1の場合は取得失敗
long get_file_size(const char *file) {
    struct stat statBuf;
    if (stat(file, &statBuf) == 0)
        return statBuf.st_size;
    return -1L;
}

int read_file_binary(const char* req_path, char* fb_buf) {
    char path[128];
    FILE *fp;
    long fsize;

    // ファイルポインタを取得 fopen()
    sprintf(path, "datas/%s", req_path);
    if ((fsize = get_file_size(path)) == -1L ||
        (fp = fopen(path, "rb")) == NULL) {
        printf("file open error!!\n");
        return -1;
    }

    // ファイルデータ（バイナリ）を読み込み
    int r = fread(fb_buf, sizeof(fb_buf[0]), fsize+2, fp);
    fclose(fp);

    // ファイルサイズ（？）を返す。
    return r;
}




