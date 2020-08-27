#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void) {
    int sockfd, nbytes;
    char buf[BUFSIZ];
    char *mesg = "Hello, world";
    struct sockaddr_in servaddr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket"); exit(1);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(10000);
    if(inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) < 0) {
        perror("inet_pton"); exit(1);
    }
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect"); exit(1);
    }
    nbytes = write(sockfd, mesg, strlen(mesg)+1);
    nbytes = read(sockfd, buf, sizeof(buf)-1);
    buf[nbytes] = '\0';
    puts(buf);
    close(sockfd);
    return 0;
}
    
