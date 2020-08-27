#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
    int sockfd, nbytes;
    char *host = "127.0.0.1";
    int port = 10000;
    char *path = "/";
    char http_msg_line[256];
    char buf[BUFSIZ];
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket"); exit(1);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(port);
    // 引数からpathを読み込む。（なければ初期値： "/"）
    if (argc > 1) {
        path = argv[1];
    }
    printf("http://%s:%d%s にHTTPリクエストします。\n\n", host, port, path);
    if (inet_pton(AF_INET, host, &servaddr.sin_addr) < 0) {
        perror("inet_pton"); exit(1);
    }

    puts("httpリクエストを作成中...");
    // Connect
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect"); exit(1);
    }
    // Read / Write
    sprintf(http_msg_line, "GET %s HTTP/1.0\r\nHost: %s:%d\r\n\r\n", path, host, port);
    puts(http_msg_line);

    // sprintf(http_msg_line, "GET %s HTTP/1.0\r\n", path);
    // puts(http_msg_line);
    // write(sockfd, http_msg_line, strlen(http_msg_line)+1);

    // sprintf(http_msg_line, "Host: %s:%d\r\n", host, port);
    // puts(http_msg_line);
    // write(sockfd, http_msg_line, strlen(http_msg_line)+1);

    // sprintf(http_msg_line, "\r\n");
    write(sockfd, http_msg_line, strlen(http_msg_line)+1);

    nbytes = read(sockfd, buf, sizeof(buf)-1);
    buf[nbytes] = '\0';
    puts(buf);
    // Close
    close(sockfd);
    return 0;
}


// 参考: https://qiita.com/edo_m18/items/cef278d0c14d1371db3b

