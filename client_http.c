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
    char buf[BUFSIZ];
    char *host;
    int port = 80;
    // int port = 10000;
    char *path;
    char *http_msg;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket"); exit(1);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(port);
    // 引数でURLが指定されていたら
    if (argc > 2) {
        host = argv[1];
        path = argv[2];
    } else {
        host = "ie.u-ryukyu.ac.jp";  // "10.3.4.7";
        path = "/";
    }
    printf("http://%s:%d%s にアクセスします。\n\n", host, port, path);
    if (inet_pton(AF_INET, host, &servaddr.sin_addr) < 0) {
        perror("inet_pton"); exit(1);
    }

    // Connect
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect"); exit(1);
    }
    // Read / Write
    sprintf(http_msg, "GET %s HTTP/1.0\r\n", path);
    write(sockfd, http_msg, strlen(http_msg));

    sprintf(http_msg, "Host: %s:%d\r\n", host, port);
    printf(http_msg);
    write(sockfd, http_msg, strlen(http_msg));

    sprintf(http_msg, "\r\n");
    write(sockfd, http_msg, strlen(http_msg));

    // sprintf(http_msg, "GET %s HTTP/1.0\r\nHost: %s:%d\r\n\r\n", path);
    nbytes = write(sockfd, http_msg, strlen(http_msg)+1);
    nbytes = read(sockfd, buf, sizeof(buf)-1);
    buf[nbytes] = '\0';
    puts(buf);
    // Close
    close(sockfd);
    return 0;
}


// 参考: https://qiita.com/edo_m18/items/cef278d0c14d1371db3b

