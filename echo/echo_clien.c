#include<stdio.h>
#include<assert.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdlib.h>
#define MAXLINE 1024


ssize_t Writen(int fd, const void *vptr, size_t n){
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
					return -1;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}

ssize_t readline(int fd, void *vptr, size_t maxlen){
	ssize_t n,rc;
	char c, *ptr;
	ptr = vptr;
	for(n = 1; n < maxlen; n++){
	again:
			if((rc = read(fd, &c, 1)) == 1){
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
void str_cli(FILE *fp, int sockfd){
	char sendline[MAXLINE], recvline[MAXLINE];

	while(fgets(sendline, MAXLINE, fp) != NULL){
		Writen(sockfd, sendline, strlen(sendline));
		if(readline(sockfd, recvline, MAXLINE) == 0)
		{
			printf("echo errno");
			exit(0);
		}
		fputs(recvline, stdout);
	}

}

int main(int argc, char* argv[]){
	if(argc != 3){
		printf("ip address");
		return 0;
	}
	int port = atoi(argv[2]);
	int sockfd = socket(PF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	int ret = inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	//assert(ret == 0);
	if(ret != 1){
		printf("%d",ret);
	}
	servaddr.sin_port = htons(port);

	ret = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	assert(ret == 0);
	printf("connected----------------------");
	
	str_cli(stdin,sockfd);

	exit(0);
}
