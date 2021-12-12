#include"unp.h"
#include<sys/socket.h>
#include<assert.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<sys/types.h>
#include<errno.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#define MAXLINE 1024


void sig_child(int signo){
	pid_t pid;
	int stat;

	while((pid = waitpid(-1,&stat, WNOHANG)) > 0){									
			printf("child %d terminated\n", pid);
	}
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
	mysignal(SIGCHLD, sig_child);
	for(;;){
		clilen = sizeof(cliaddr);
		if(connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen) < 0){
			if(errno == EINTR)
					continue;
			else{
					printf("accept errno");
					exit(0);
			}
		}
		if((childpid = fork())== 0){
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		close(connfd);
	}

}
