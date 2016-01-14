#pragma once
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<net/if.h>
#include<net/route.h>
#include<netinet/in.h>
#include<errno.h>
#include<strings.h>
#include<string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include<fcntl.h>
#include<net/if.h>
#include<sys/ioctl.h>
#include<sys/sysctl.h>
#include<sys/param.h>
#include<sys/utsname.h>
#include<pthread.h>
#include <ctype.h>
#include<unistd.h>

#include <strings.h>

#define SA struct sockaddr
#define SIN struct sockaddr_in
#define RTM struct rt_msghdr
#define IFM struct if_msghdr
#define IFA struct ifa_msghdr
#define IFMA struct ifma_msghdr
#define IFAN struct if_announcemsghdr 

#define MAXLEN 2048//1024
#define SEQNO 9999
#define IN 1
#define OUT 0

//void *process_file(void *arg);
//void *convert_case(void *arg);
//void *word_count(void *arg);
//void trim (char *s, char *res);

struct word {
	char name[20];
	int count;
};

struct stats {
	char *content;
	char fileName[MAXLEN];
	int characters;
	int alpha;
	int digit;
	int words;
	int sentences;
	int lines;
	int alpha_count[26];
	struct word w[1000];
	pthread_rwlock_t *mut;
	struct timeval elapsed;
};
