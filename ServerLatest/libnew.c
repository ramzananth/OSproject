#include "header.h"
/*
 * Author: Nandthitha Thiagarajan
 * Date: 11/17/2013
 * 
 */

void send_info_to_client (struct stats stat, int conn) {
	char sendbuff[1024];
	int i, j;

	write(conn, &stat, sizeof(stat));
	//	memset(sendbuff, '0', sizeof(sendbuff));
	//	snprintf(sendbuff, sizeof(sendbuff), "others  cSentences %d",stat.sentences);
	//	write(conn, sendbuff, strlen(sendbuff));

	//	snprintf(sendbuff, sizeof(sendbuff), "others  cCharacters %d",stat.characters);
	//	write(conn, sendbuff, strlen(sendbuff));
	//
	//	snprintf(sendbuff, sizeof(sendbuff), "others  cAlpha %d",stat.alpha);
	//	write(conn, sendbuff, strlen(sendbuff));
	//
	//	snprintf(sendbuff, sizeof(sendbuff), "others  cWords %d",stat.words);
	//	write(conn, sendbuff, strlen(sendbuff));
	//
	//	snprintf(sendbuff, sizeof(sendbuff), "filename  %s 1",stat.fileName);
	//	write(conn, sendbuff, strlen(sendbuff));
	//
	//	for (i = 65, j = 0; i <=90, j<=25; i++,j++) {
	//		snprintf(sendbuff, sizeof(sendbuff), "cCI  %c %d",i, stat.alpha_count[j]);
	//		write(conn, sendbuff, strlen(sendbuff));
	//	}
	//
	//	for (i=0;stat.w[i].name[0];i++) {
	//		snprintf(sendbuff, sizeof(sendbuff), "cWI  %s %d",stat.w[i].name, stat.w[i].count);
	//		write(conn, sendbuff, strlen(sendbuff));
	//	}

}

void *process_file(void *arg) {
	int conn,status;
	int i = 65, j = 0;
	struct stats serverStat;
	size_t size;
	ssize_t n;
	char *line[MAXLEN], *content;
	FILE *file;
	pthread_t tid1, tid2, tid3, tid4,tid5, tid6, tid7;
	void *ret;
	char input[MAXLEN];
	conn = *((int *)arg);
	free(arg);
	struct timeval before, after;
	float elapsed;

	pthread_detach(pthread_self());
	//while ((n = read(conn, input, MAXLEN))>0);
	n = read(conn, input, MAXLEN);
	printf("received from client %s\n", input);
	gettimeofday (&before, NULL);

	file = fopen(input, "r+");
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);
	content = malloc(size * (sizeof(char)));
	fread(content, sizeof(char), size, file);
	serverStat.content = content;

	serverStat.mut = (pthread_rwlock_t*) malloc (sizeof(pthread_rwlock_t));
	pthread_rwlock_init (serverStat.mut, NULL);

	serverStat.characters = serverStat.alpha = serverStat.digit = serverStat.words = serverStat.sentences = 0;
	for (i=0;i<26;i++)
		serverStat.alpha_count[i] = 0;

	printf ("\nContent of the file before manipulation:\n%s\n\n", serverStat.content);

	if((status = pthread_create(&tid1,NULL,&char_count,&serverStat)) != 0) {
		printf("Thread creation for counting characters failed due to %s\n",strerror(status));
		exit(1);
	}
	if((status = pthread_create(&tid2,NULL,&alphabet_count,&serverStat)) != 0) {
		printf("Thread creation for counting alphabets failed due to %s\n",strerror(status));
		exit(1);
	}
	if((status = pthread_create(&tid3,NULL,&digit_count,&serverStat)) != 0) {
		printf("Thread creation for counting digits failed due to %s\n",strerror(status));
		exit(1);
	}
	if((status = pthread_create(&tid4,NULL,&sentence_count,&serverStat)) != 0) {
		printf("Thread creation for counting sentences failed due to %s\n",strerror(status));
		exit(1);
	}
	if((status = pthread_create(&tid5,NULL,&word_count,&serverStat)) != 0) {
		printf("Thread creation for counting occurence of each word failed due to %s\n",strerror(status));
		exit(1);
	}
	if((status = pthread_create(&tid6,NULL,&line_count,&serverStat)) != 0) {
		printf("Thread creation for counting number of lines failed due to %s\n",strerror(status));
		exit(1);
	}
	if((status = pthread_create(&tid7,NULL,&convert_case,&serverStat)) !=0) {
		printf("Thread creation for character case conversion failed due to %s\n",strerror(status));
		exit(1);
	}
	pthread_join(tid1, &ret);
	pthread_join(tid2, &ret);
	pthread_join(tid3, &ret);
	pthread_join(tid4, &ret);
	pthread_join(tid5, &ret);
	pthread_join(tid6, &ret);
	pthread_join(tid7, &ret);
	pthread_rwlock_destroy (serverStat.mut);
	gettimeofday (&after, NULL);

	// calculating sever performance time
	serverStat.elapsed.tv_sec = after.tv_sec - before.tv_sec;
	serverStat.elapsed.tv_usec = after.tv_usec - before.tv_usec;
	elapsed =  serverStat.elapsed.tv_sec*1e3 + serverStat.elapsed.tv_usec/1E3;

	printf ("Content of the file after manipulation:\n%s\n\n", serverStat.content);
	printf("########server-side statistics:########\n\n");
	printf ("Number of characters is %d\n", serverStat.characters);
	printf ("Number of alphabets is %d\n", serverStat.alpha);
	printf ("Number of digit is %d\n", serverStat.digit);
	printf ("Number of words is %d\n", serverStat.words);
	printf ("Number of sentences is %d\n", serverStat.sentences);
	printf ("Number of lines is %d\n", serverStat.lines);
	printf ("Number of occurences of each alphabet: \n");
	for (i = 65, j = 0; i <=90, j<=25; i++,j++)
		printf("%c : %d\n",i,serverStat.alpha_count[j]);
	printf ("Number of occurences of each word: \n");
	for (i=0;serverStat.w[i].name[0];i++)
		printf("%s : %d\n",serverStat.w[i].name,serverStat.w[i].count);
	printf("\nTime taken to satisfy client request is %5.4f (millisecs)\n",elapsed);

	fclose(file);

	int randomAppend = 10000+(rand()%10000);
	char uuid[10];
	sprintf(uuid,".%d",randomAppend);

	strcpy(serverStat.fileName,input);
	strcat(serverStat.fileName,uuid);
	strcat(serverStat.fileName,".server.txt");

	//write converted case to client
	file = fopen(serverStat.fileName, "w+");
	fwrite(serverStat.content, sizeof(char), size, file);
	fclose(file);

	//statistics result to client
	send_info_to_client(serverStat, conn);

	//	write(conn, &serverStat, sizeof(serverStat));

	//close client connection
	close(conn);
	return NULL;
}

void *convert_case(void *arg) {
	struct stats *stat = (struct stats *)arg;
	char c;
	int i = 0;

	pthread_rwlock_wrlock(stat->mut);
	while ((stat->content[i])) {
		c = stat->content[i];
		if(c == ' ' || c == '\n' || c == '\t' || c == '.' || c == ','){
			stat->content[i] = c;
		}
		else
			if(isalpha(c)) {
				if (islower(c)) {
					stat->alpha_count[(c-'a')] += 1;
					stat->content[i] = toupper(c);
				} else {
					stat->alpha_count[(c-'A')] += 1 ;
					stat->content[i] = tolower(c);
				}
			}
			else
			{
				//stat->content[i] = ' ';
			}
		i++;
	}
	pthread_rwlock_unlock(stat->mut);
}

void *char_count(void *arg) {
	struct stats *stat = (struct stats *)arg;
	int i = 0;

	pthread_rwlock_rdlock(stat->mut);
	while ((stat->content[i])) {
		stat->characters += 1;
		i++;
	}
	pthread_rwlock_unlock(stat->mut);
}


void *alphabet_count (void *arg) {
	struct stats *stat = (struct stats *)arg;
	char c;
	int i = 0;

	pthread_rwlock_rdlock(stat->mut);
	while ((stat->content[i])) {
		c = stat->content[i];
		if(isalpha(c)) {
			stat->alpha += 1;
		}
		i++;
	}
	pthread_rwlock_unlock(stat->mut);
} 

void *digit_count(void *arg) {
	struct stats *stat = (struct stats *)arg;
	char c;
	int i = 0;

	pthread_rwlock_rdlock(stat->mut);
	while ((stat->content[i])) {
		c = stat->content[i];
		if (isdigit(c)) {
			stat->digit +=1;
		}
		i++;
	}
	pthread_rwlock_unlock(stat->mut);
}

void *sentence_count(void *arg) {
	struct stats *stat = (struct stats *)arg;
	char c;
	int i = 0,state = OUT;

	pthread_rwlock_rdlock(stat->mut);
	while ((stat->content[i])) {
		c = stat->content[i];
		if (c == '.') {
			stat->sentences += 1;
			if (stat->content[i+1] != ' ')
				state = OUT;
		}
		if (c == ' ' || c == '\n' || c == '\t') {
			state = OUT;
		} else if (state == OUT) {
			state = IN;
			stat->words += 1;
		}
		i++;
	}
	pthread_rwlock_unlock(stat->mut);
}

void *line_count(void *arg) {
	struct stats *stat = (struct stats *)arg;
	char c;
	int i = 0;

	pthread_rwlock_rdlock(stat->mut);
	while ((stat->content[i])) {
		c = stat->content[i];
		if (c == '\n' || c == '\r') {
			stat->lines += 1;
		}
		i++;
	}
	pthread_rwlock_unlock(stat->mut);

}


void *word_count(void *arg) {

	//pthread_detach(pthread_self());

	char *str1,*str2, *input,*token,*subtoken, after_trim[20];
	char *saveptr1, *saveptr2;
	int j=1,count=0,i=0,found=0, start=0,end=0;
	struct stats *stat = (struct stats *)arg;

	pthread_rwlock_rdlock(stat->mut);
	input = malloc(strlen(stat->content)*sizeof(char));
	strcpy(input,stat->content);
	pthread_rwlock_unlock(stat->mut);

	for (j = 1, str1 = input; ; j++, str1 = NULL) {
		token = strtok_r(str1,"\n", &saveptr1);
		if (token == NULL)
			break;

		for (str2 = token;;str2 = NULL) {
			found = 0;
			subtoken = strtok_r(str2," ", &saveptr2);
			if (subtoken == NULL)
				break;
			if ((subtoken[(strlen(subtoken)-1)] == '.') || (subtoken[(strlen(subtoken)-1)] == ','))
				subtoken[(strlen(subtoken)-1)] = '\0';

			start = (!isalpha(subtoken[0]) && !isdigit(subtoken[0])) ? 1 : 0;
			end = (!isalpha(subtoken[0]) && !isdigit(subtoken[0])) ? strlen(subtoken)-2 : strlen(subtoken)-1;
			if(end >= start && (start || (end < (strlen(subtoken)-1)))) {
				bcopy(subtoken+start,subtoken,(end-start)+1);
				if(!start && end < strlen(subtoken)-1) {
					subtoken[end+1] = '\0';
				} else {
					subtoken[end] = '\0';
				}
			}
			for (i=0;i<=count;i++) {
				if (!strcmp(subtoken,stat->w[i].name)) {
					stat->w[i].count++;
					found = 1;
				}
			}
			if (!found) {
				strcpy(stat->w[count].name,subtoken);
				stat->w[count].count++;
				count++;
			}
		}
	}
	stat->w[i].name[0] = '\0';

}
