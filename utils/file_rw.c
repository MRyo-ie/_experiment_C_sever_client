#include "file_rw.h"

//ファイルサイズを取得する
//戻り値が-1の場合は取得失敗
long get_file_size(const char *file) {
    struct stat statBuf;
    if (stat(file, &statBuf) == 0) return statBuf.st_size;
    return -1L;
}

int read_file_binary(char *req_path, int *fb_buf) {
    char path[128];
    FILE *fp;
    long fsize;

    sprintf(path, "datas/%s", req_path);
    if ((fsize = get_file_size(path)) == -1L ||
        (fp = fopen(path, "r")) == NULL) {
        printf("file open error!!\n");
        exit(1);
    }

    // ファイルデータを読み込み
    int r = fread(fb_buf, sizeof(fb_buf[0]), BUFSIZ, fp);
    fclose(fp);

    return 0;
}
