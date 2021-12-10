#include<sys/socket.h>
#include<assert.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/types.h>
#include<errno.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#define MAXLINE 1024
ssize_t Writen(int fd, const void* vptr, size_t n){
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0){
		if((nwritten = write(fd, ptr, nleft)) <= 0){
			if(nwritten < 0 && errno == EINTR)
							nwritten = 0;
			else
							return(-1);
		}
		nleft -= nwritten;
		ptr += nwritten;
	
	}
	return n;
}

ssize_t readline(int sockfd, void *vptr, size_t maxlen){
	ssize_t n,rc;
	char c, *ptr;

	ptr = vptr;
	for(n = 1; n < maxlen; n++){
	again:
		if((rc = read(sockfd, &c, 1)) == 1){
			*ptr++ = c;
			if(c == '\n')
				break;
		}else if(rc == 0){
				*ptr = 0;
				return n-1;
		}else{
			if(errno == EINTR)
				goto again;
			return -1;
		}
	
	}
	*ptr = 0;
	return n;
}

void sig_child(int signo){
	pid_t pid;
	int stat;

	pid = wait(&stat);
	printf("child %d terminated\n", pid);
	return;
}
void str_echo(int sockfd){
	ssize_t n;
	char buf[1024];

again:
	while((n = readline(sockfd, buf, MAXLINE)) > 0)
					Writen(sockfd, buf, n);
	if(n < 0 && errno == EINTR)
					goto again;
	else if(n < 0)
					printf("str_echo: read error");


}

int main(int argc, char*argv[]){
	pid_t childpid;
	int listenfd, connfd;
	socklen_t clilen;

	struct sockaddr_in cliaddr, servaddr;
	
	int port = atoi(argv[2]);
	const char *ip = argv[1];

	listenfd = socket(PF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &servaddr.sin_addr);

	int ret = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	assert(ret == 0);
	listen(listenfd,5);
	signal(SIGCHLD, sig_child);
	for(;;){
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
		if((childpid = fork())== 0){
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		close(connfd);
	}

}
