#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

#include "header.h"
#include "TextFileStats.h"

#define WRITE_FILE_SEMAPHORE "/semaphoreClient"

#define TIMING_FILE "./performanceTimes.txt"

#define CASE_CONVERSION_FORMAT "./testServerCaseConversion%d.txt"

#define NUMBER_OF_PARALLEL_CLIENTS 100
int filter_dir(const struct dirent * entry)
{
	if(entry->d_type & DT_DIR)
		return 0;
	else
	{
		if (!strstr(entry->d_name,".txt"))
		{
			return 0;
		}
		if (strstr(entry->d_name,".server"))
		{
			return 0;
		}
		if (strstr(entry->d_name,".txt"))
		{
			return 1;
		}
	}
}

int main (int argc,char **argv) {
	int sockfd;
	socklen_t len;
	struct sockaddr_in serv_addr, cli_addr, local_addr;
	pid_t pid;

	//clear performance time files
	FILE* fd = fopen(TIMING_FILE,"w+");
	fclose(fd);

	//create write semaphore
	sem_t * commonSemaphore;
	commonSemaphore = sem_open(WRITE_FILE_SEMAPHORE,O_CREAT||O_EXCL, 0644, 1);
	sem_unlink(WRITE_FILE_SEMAPHORE);

	char inputFileName[1024];
	if (argc != 4) {
		printf("need to specify address, port number and Directory\n");
		printf("Where test files are located\n");
		exit(1);
	}

	bzero (&cli_addr, sizeof(cli_addr));
	bzero (&serv_addr, sizeof(serv_addr));
	bzero (&local_addr, sizeof(local_addr));

	int i;
	float timeTaken[NUMBER_OF_PARALLEL_CLIENTS];
	/* For random file generation */
	struct dirent ** namelist;
	int n;

	printf ("Directory name is \\%s\\\n",argv[3]);

	n = scandir(argv[3], &namelist, &filter_dir, 0);
	if (n < 0) {
		perror("scandir \n");
		exit(1);
	}
	printf("There are %d files in given directory %s \n",n,argv[3]);
	for(i=0; i<n; i++)
	{
		printf("%s\n", namelist[i]->d_name);
	}





	for (i = 0; i < NUMBER_OF_PARALLEL_CLIENTS; i++)
	{

		pid = fork();

		if (pid > 0)
		{

			continue;
		}
		if (pid < 0)
		{

			continue;
		}

		struct timeval tvRandomize = {0};
		gettimeofday(&tvRandomize,NULL);
		srand(100+tvRandomize.tv_usec*100);//set new random seed in each child process
		int fileIndex = rand()%n;
		strcpy(inputFileName, argv[3]);
		strcat(inputFileName,"/");
		strcat(inputFileName, namelist[fileIndex]->d_name);
		printf("chosen input file %s \n",inputFileName);

		//create socket
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			printf("socket creation failed due to %s\n",strerror(errno));
			exit(1);
		}

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(atoi(argv[2]));
		inet_pton(AF_INET,argv[1] ,&serv_addr.sin_addr);

		struct timeval tvBegin = {0};
		struct timeval tvEnd = {0};
		gettimeofday(&tvBegin,NULL);

		//connect to server
		if (connect(sockfd, (SA *) &serv_addr, sizeof(serv_addr)) == -1) {
			printf("Connect failed due to %s\n",strerror(errno));
			exit(1);
		}
		len = sizeof(cli_addr);
		getsockname(sockfd, (SA *)&cli_addr,&len);

		write(sockfd, inputFileName, MAXLEN);

		struct stats thisClientStats;
		read(sockfd, &thisClientStats, sizeof(thisClientStats));

		gettimeofday(&tvEnd,NULL);

		float elapsedTimeMilliSecs = (tvEnd.tv_sec-tvBegin.tv_sec)*1e3 + (tvEnd.tv_usec-tvBegin.tv_usec)/1E3;
		timeTaken[i] = elapsedTimeMilliSecs;

		printf("received result from server, elapsed time %5.4f (millisecs) \n",elapsedTimeMilliSecs);

		sem_wait(commonSemaphore);
		fd = fopen(TIMING_FILE,"a");
		fprintf(fd,"%f\n",elapsedTimeMilliSecs);
		fclose(fd);

		//test server result here
		int charCount, wordCount, sentenceCount;
		int alphabetCount[26] = {0};
		struct word wordInfo[1000];
		char caseConversionFileName[1024];
		sprintf(caseConversionFileName,CASE_CONVERSION_FORMAT,i);

		//printf("trying to validate server result : computing client side stats \n");
		countCharWordsSentCase(inputFileName,
				caseConversionFileName,
				&charCount,
				&wordCount,
				&sentenceCount);

		//		cal_word_info(inputFileName, wordInfo);
		cal_char_info(inputFileName, alphabetCount);
		int compareConversionFiles = 0;
		compareConversionFiles = compareFiles(caseConversionFileName,thisClientStats.fileName);

		//printf("trying to validate server result : comparing results \n");
		if ( (thisClientStats.sentences == sentenceCount) &&
				//(thisClientStats.words == wordCount) &&
				(thisClientStats.characters == charCount)
				//&& (compareConversionFiles == 0)
				)
		{
			printf("server results (sentence count, and character count) matches clients testing result for client %d \n",i);
		}
		else
		{
			printf("server results (sentence count and character count) does not match clients testing result for client %d \n",i);
//			printf("error in client run %d \n",i);
//			if ((thisClientStats.sentences != sentenceCount))
//			{
//				printf("sentence count does not match server %d, client %d\n",
//						thisClientStats.sentences, sentenceCount);
//			}
//
//			if ((thisClientStats.words != wordCount))
//			{
//				printf("word count does not match  server %d, client %d\n",
//						thisClientStats.words, wordCount);
//			}
//			if ((thisClientStats.characters != charCount))
//			{
//				printf("char count does not match server %d, client %d\n",
//						thisClientStats.characters, charCount);
//			}
//			//now compare files
//			if (compareConversionFiles != 0)
//			{
//				printf("case conversion was in error \n",i);
//			}

		}

		if (pid == 0)
		{
			exit(0);
		}
	}


	//wait for all children to finish writing
	while (wait(NULL) > 0) { ;};

	sem_close(commonSemaphore);//close semaphore
	//get statistics
	float elapsedTimeMilliSecs, averageTime, minTime, maxTime;
	minTime = 1E10;
	maxTime = -1E10;
	averageTime = 0.0f;
	fd = fopen(TIMING_FILE,"r");
	for (i = 0; i < NUMBER_OF_PARALLEL_CLIENTS; i++)
	{
		fscanf(fd,"%f",&elapsedTimeMilliSecs);
		//printf("read value %f\n",elapsedTimeMilliSecs);
		averageTime += elapsedTimeMilliSecs;
		if (minTime > elapsedTimeMilliSecs)
		{
			minTime = elapsedTimeMilliSecs;
		}
		if (maxTime < elapsedTimeMilliSecs)
		{
			maxTime = elapsedTimeMilliSecs;
		}
	}
	fclose(fd);

	averageTime /= NUMBER_OF_PARALLEL_CLIENTS;

	printf("Minimum processing time when %d processes are launched : %f (millisecs) \n",NUMBER_OF_PARALLEL_CLIENTS,minTime);
	printf("Maximum processing time when %d processes are launched : %f (millisecs) \n",NUMBER_OF_PARALLEL_CLIENTS,maxTime);
	printf("Average processing time when %d processes are launched : %f (millisecs) \n",NUMBER_OF_PARALLEL_CLIENTS,averageTime);


}
