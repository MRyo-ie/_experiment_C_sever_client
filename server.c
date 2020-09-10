#include "server.h"

RETSIGTYPE reaper(int sn) {
    pid_t cpid; int cr;
    do {
    try_waitpid:
        cpid=waitpid((pid_t)-1, &cr,
                WNOHANG|WUNTRACED);
        if(cpid<0) {
            if(errno==EINTR){goto try_waitpid;}}
        if(cpid>0) {
            debug_print("Info", "reaper!", true);
            debug_print_int("pid", cpid, false);
            debug_print_int("ret", cr, true);
        }
    } while(cpid>0);
}


int main(void) {
    int listenfd, connfd, nbytes;
    char http_msg[BUFSIZ];
    struct http_req_template parsed_http_req;
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
    servaddr.sin_port = htons(LISTEN_HOST);
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

    // 初期化完了
    printf("[Info] サーバー初期化完了！\n");
    printf("[Info] http://%s:%d/ を listen しています。\n",
                LISTEN_DOMAIN, LISTEN_HOST);
    //***  accept ~ close ループ  ***//
    for (;;) {
        int pid;
        // accept
        connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
        pid = fork();
        if (pid == 0) {
            close(listenfd);
            while ((nbytes = read(connfd, http_msg, sizeof(http_msg))) > 0) {
                debug_print("Info", "HTTP Request ↓ (http_msg)", true);
                printf("%s\n", http_msg);  // HTTPリクエストを表示
                parse_HTTP_req(http_msg, &parsed_http_req);
                // ファイル読み込み
                // read_file_binary("test.txt", fb_buf);
                // printf("%s \n", fb_buf);
                // write
                write(connfd, fb_buf, sizeof(fb_buf) - 1);
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

