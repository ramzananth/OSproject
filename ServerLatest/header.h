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
#include<sys/time.h>
#include<pthread.h>
#include <ctype.h>
#include<unistd.h>
#define SA struct sockaddr
#define SIN struct sockaddr_in

#define MAXLEN 2048
#define SEQNO 9999
#define IN 1
#define OUT 0

void *process_file(void *arg);
void *convert_case(void *arg);
void *word_count(void *arg);
void trim (char *s, char *res);
void *digit_count(void *arg);
void *sentence_count(void *arg);
void *char_count(void *arg);
void *alphabet_count (void *arg);
void *line_count(void *arg);

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

