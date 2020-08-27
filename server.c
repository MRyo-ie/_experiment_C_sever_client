#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>

#define RETSIGTYPE void

RETSIGTYPE reaper(int sn) {
    pid_t cpid;
    int cr;
    do {
    try_waitpid:
        cpid = waitpid((pid_t)-1, &cr, WNOHANG | WUNTRACED);
        if (cpid < 0) {
            if (errno == EINTR) {
                goto try_waitpid;
            }
        }
        if (cpid > 0) {
            printf("pid#%d,ret %d\n", cpid, cr);
        }
    } while (cpid > 0);
}

//ファイルサイズを取得する
//戻り値が-1の場合は取得失敗
long get_file_size(const char *file) {
    struct stat statBuf;
    if (stat(file, &statBuf) == 0) 
        return statBuf.st_size;
    return -1L;
}

int read_file_binary(char *req_path, int *fb_buf) {
    FILE *fp;
    char path[128];
    long fsize;

    sprintf(path, "datas/%s", req_path);
    printf("fsize : %d\n", fsize);
    if ((fsize = get_file_size(path)) == -1L || (fp = fopen(path, "r")) == NULL) {
        printf("file open error!!\n");
        exit(1);
    }

    /* (4)ファイルの読み（書き）*/
    int r = fread(fb_buf, sizeof(fb_buf[0]), BUFSIZ, fp);
    // while (() < fsize) {
    //     /* ここではfgets()により１行単位で読み出し */
    //     printf("%s  : %d\n", fb_buf, r);
    //     break;
    // }
    fclose(fp); /* (5)ファイルのクローズ */

    return 0;
}

int main(void) {
    int listenfd, connfd, nbytes;
    char http_msg[BUFSIZ];
    int fb_buf[BUFSIZ];
    struct sockaddr_in servaddr;

    //***  初期化  ***//
    signal(SIGCHLD, reaper);
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(10000);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // bind
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        exit(1);
    }
    // listen
    if (listen(listenfd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    //***  accept ~ close ループ  ***//
    for (;;) {
        int pid;
        // accept
        connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
        pid = fork();
        if (pid == 0) {
            close(listenfd);
            while ((nbytes = read(connfd, http_msg, sizeof(http_msg))) > 0) {
                // ファイル読み込み
                read_file_binary("test.txt", fb_buf);
                printf("%s \n", fb_buf);
                // write
                write(connfd, fb_buf, sizeof(fb_buf)-1);
            }
            // close
            close(connfd);
            exit(0);
        } else {
            // close
            close(connfd);
        }
    }
    return 0;
}
