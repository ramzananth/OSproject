#include "header.h"
/*
 * Author: Nandthitha Thiagarajan
 * Date: 11/17/2013
 *
 */

int  a =10;
void close_fd (int fd) {
	close(fd);
}

int main (int argc,char **argv) {
	int sockfd, *connfd;
	socklen_t len;
	struct sockaddr_in serv_addr, cli_addr,local_addr;
	char buf[100];
	pthread_t tid;

	if (argc != 2) {
		printf("need to specify port number\n");
		exit(1);
	}

	bzero (&serv_addr, sizeof(serv_addr));
	bzero (&cli_addr, sizeof(cli_addr));
	bzero (&local_addr, sizeof(local_addr));

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("socket creation failed due to %s\n",strerror(errno));
		exit(1);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[1]));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockfd, (SA *) &serv_addr, sizeof(serv_addr)) ==-1  ) {
		printf("Bind failed due to %s\n",strerror(errno));
		close_fd(sockfd);
		exit(1);
	}

	if (listen(sockfd, 1024) ==-1  ) {
		printf("Listen failed due to %s",strerror(errno));
		exit(1);
	}

	while(1) {
		len = sizeof(cli_addr);
		connfd = malloc(sizeof(int));
		if ((*connfd = accept(sockfd,(SA *)&cli_addr, &len)) == -1) {
			printf("accept failed due to %s\n",strerror(errno));
			exit(1);
		}
		printf("Accept from client successful\n");
		pthread_create(&tid,NULL,&process_file,connfd);
	}


}
