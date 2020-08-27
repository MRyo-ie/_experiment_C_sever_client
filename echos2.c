#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#define RETSIGTYPE void

RETSIGTYPE reaper(int sn) {
  pid_t cpid; int cr;
  do {
try_waitpid:
   cpid=waitpid((pid_t)-1, &cr,
          WNOHANG|WUNTRACED);
   if(cpid<0) {
     if(errno==EINTR){goto try_waitpid;}}
   if(cpid>0) {
     printf("pid#%d,ret %d\n", cpid,cr);}
  } while(cpid>0);
}

int main(void) {
    int listenfd, connfd, nbytes;
    char buf[BUFSIZ];
    struct sockaddr_in servaddr;

    signal(SIGCHLD, reaper);
    if((listenfd= socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket"); exit(1);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(10000);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))<0) {
        perror("bind"); exit(1);
    }
    if(listen(listenfd, 5) < 0 ) {
        perror("listen"); exit(1);
    }
    for(;;) {
        int pid;
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        pid = fork();
        if(pid==0) {
            close(listenfd);
            while( (nbytes = read(connfd, buf, sizeof(buf))) > 0) {
                write(connfd, buf, nbytes);
            }
            close(connfd);
            exit(0);
        }
        else {
            close(connfd);
        }
    }
    return 0;
}
