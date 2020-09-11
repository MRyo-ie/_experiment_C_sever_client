#include "server.h"

static const bool is_debug_mode = true;

RETSIGTYPE reaper(int sn) {
    pid_t cpid; int cr;
    do {
    try_waitpid:
        cpid=waitpid((pid_t)-1, &cr,
                WNOHANG|WUNTRACED);
        if(cpid<0) {
            if(errno==EINTR){goto try_waitpid;}}
        if(cpid>0) {
            debug_print("Info", "reaper!", true, is_debug_mode);
            debug_print_int("pid", cpid, false, is_debug_mode);
            debug_print_int("ret", cr, true, is_debug_mode);
        }
    } while(cpid>0);
}


int main(void) {
    int listenfd, connfd, nbytes;
    char http_msg[BUFSIZ];
    http_req parsed_http_req;  // parse HTTP Request
    http_res build_http_res;  // build HTTP Response
    char buf[BUFSIZ*100];
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
                debug_print("Info", "HTTP Request ↓", true, is_debug_mode);
                debug_print_msg(http_msg, true, is_debug_mode);  // HTTPリクエストを表示
                // HTTP リクエストを http_req 構造体に分解 (parse) する。
                parse_HTTP_req(http_msg, &parsed_http_req);
                // print_struct_req("server.c", &parsed_http_req);
                // http_req の情報を元に、http_res を作成 (build) する。
                analyze_path(&parsed_http_req, &build_http_res);
                // debug
                print_struct_res("server.c", &build_http_res, false);
                build_HTTP_res(&parsed_http_req, &build_http_res, buf);
                // write
                write(connfd, buf, sizeof(buf) - 1);
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

