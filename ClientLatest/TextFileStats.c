/*
 * TextFileStats.c
 *
 *  Created on: Nov 16, 2013
 *      Author: ramyaananth
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "TextFileStats.h"

void countCharWordsSentCase(char fileName[], char fileName2[], int* charCount, int* wordCount, int* sentenceCount)
{
	//char c;
	char ch;
	FILE *fp1, *fp2;
	int no_of_char = 0, no_of_words = 1, no_of_sent = 0;
	fp1= fopen(fileName,"r");
	if( fp1 == NULL )
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	fp2 = fopen(fileName2,"w");
	if( fp2 == NULL )
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	//upper to lower and viceversa
	while (!feof(fp1)) {
		no_of_char++;
		ch = fgetc(fp1);

		if (ch == ' ' || ch == '.' || ch == '\n' || ch == '\t')
		{
			if(ch == ' ' || ch == '\n' || ch == '\t')
			{
				fputc(ch,fp2);
				no_of_words++;
			}
			else
				if(ch == '.')
				{
					fputc(ch,fp2);
					no_of_sent++;
				}
		}

		else
			if (isalpha(ch))
			{
				if (isupper(ch))
				{
					fputc(tolower(ch),fp2);
				}
				else {
					fputc(toupper(ch),fp2);
				}
			}
			else
			{
				//fputc(' ',fp2);
			}
	}
	fclose(fp1);
	fclose(fp2);

	no_of_char--;
	*charCount = no_of_char;
	*wordCount = no_of_words;
	*sentenceCount = no_of_sent;

	return;

}

int compareFiles(const char* fileName1, const char* fileName2)
{
	FILE* fp1;
	FILE* fp2;
	int returnValue = 0;
	fp1 = fopen(fileName1,"r");
	fp2 = fopen(fileName2,"r");
	while( !feof(fp1) && !feof(fp1))
	{
		char c1,c2;
		fscanf(fp1,"%c",&c1);
		fscanf(fp2,"%c",&c2);
		//printf("client side %c, server side %c \n",c1,c2);
		if (c1 != c2)
		{
			if (isalpha(c1) || isalpha(c2))
			{
			returnValue =  -1;
			break;
			}
		}
	}


	if (feof(fp1) != feof(fp2))
	{
		returnValue = -1;
	}

	fclose(fp1);
	fclose(fp2);

	return returnValue;
}



void cal_char_info(char *filename, int* data)
{
	FILE *fp;
	int count;
	int ch;
	FILE *stream;
	char event[20];
	//int data[26];
	int i;
	//	stream = fdopen (file, "w");

	fp = fopen(filename, "r");
	count = 0;

	if(fp == NULL)
	{
		printf("ERROR: unable to open the file: %s", filename);
		exit(1);
	}

	for(i=0;i<26;i++)
		data[i] = 0;

	do {
		ch = fgetc (fp);
		if(isalpha(ch)) {
			if(ch < 91) {
				data[ch - 65]++;
			} else {
				data[ch - 97]++;
			}
		}
	} while (ch != EOF);

	/*for(i=0; i<26; i++) {
		event[0] = (char) (i+65);
		event[1] = (char) '\0';

		//printf("Before writing into the pipe:\n");
		fprintf(stream,"cCI %s %d\n", event ,data[i]);
		//printf("After writing into the pipe:\n");
		//printf("cCI %s %d\n", event, data[i]);

	}
	 */
	fclose(fp);
	//	fclose(stream);
}

void cal_word_info(char *filename, struct word cWI[])
{
	FILE *fp;
	int count;

	FILE *stream;
	char event[20];
	char str[1024];
	char *cmd;
	char *temp;

	char *space_ptr;

	int i;
	//struct word_info cWI[1000];

	//stream = fdopen (file, "w");

	fp = fopen(filename, "r");
	count = 0;

	if(fp == NULL)
	{
		printf("ERROR: unable to open the file: %s", filename);
		exit(1);
	}

	//Process each line in the file
	while( fgets (str, 1024, fp) != NULL) {
		cmd = (char *) malloc (1024*sizeof(char));
		temp = (char *) malloc (1024*sizeof(char));
		strcpy(cmd, str);
		space_ptr = strchr(str, ' ');
		//Process each word in the line
		while(space_ptr != NULL) {
			*space_ptr = '\0';
			strcpy(temp,str);
			//printf("div: before %s", temp);
			if(strcmp(temp, "")== 0) {
				*space_ptr = ' ';
				//str = ++space_ptr;
				cmd = ++space_ptr;
				strcpy(str,cmd);
				space_ptr = strchr(str, ' ');
				continue;
			}
			if((temp[strlen(temp)-1] == '.') || (temp[strlen(temp)-1] == ',') ){
				temp[strlen(temp)-1] = '\0';
			}
			//printf("div: after %s", temp);
			//If this is the first word, create the info
			//Else: see if you alrady have in the cWI, else create one
			if(count == 0) {
				strcpy(cWI[0].name, temp);
				cWI[0].count = 1;
				count++;
			} else {
				int found = 0;
				for(i=0; i<count; i++) {
					if(strcmp(cWI[i].name, temp) == 0) {
						found = 1;
						cWI[i].count++;
						break;
					}
				}
				if(found == 0) {
					strcpy(cWI[count].name, temp);
					cWI[count].count = 1;
					count++;
				}
			}
			*space_ptr = ' ';
			//str = ++space_ptr;
			cmd = ++space_ptr;
			strcpy(str,cmd);
			space_ptr = strchr(str, ' ');
		}
	}

	for(i=0; i<count; i++) {
		//fprintf(stream,"cWI %s %d\n", cWI[i].name ,cWI[i].count);
		printf("cWI %s %d\n", cWI[i].name ,cWI[i].count);
	}
	//	fprintf(stream,"others tWordCount %d\n",count);

	fclose(fp);
	//	fclose(stream);
}


int cal_sentences(char *filename)
{
	FILE *fp;
	int check_for_sp_nl;
	int count;
	int ch;

	fp = fopen(filename, "r");
	count = 0;
	check_for_sp_nl = 0;

	if(fp == NULL)
	{
		printf("ERROR: unable to open the file: %s", filename);
		exit(1);
	}

	do {
		ch = fgetc (fp);
		if(check_for_sp_nl) {
			if((ch == '\n') || (ch == ' ')) count++;
			check_for_sp_nl = 0;
		}
		if(ch == '.') {
			check_for_sp_nl = 1;
		}
	} while (ch != EOF);

	fclose(fp);
	return count;
}

int cal_alphabets(char *filename)
{
	FILE *fp;
	int count;
	int ch;

	fp = fopen(filename, "r");
	count = 0;

	if(fp == NULL)
	{
		printf("ERROR: unable to open the file: %s", filename);
		exit(1);
	}

	do {
		ch = fgetc (fp);
		if(isalpha(ch)) {
			count++;
		}
	} while (ch != EOF);

	fclose(fp);
	return count;
}

int cal_lines(char *filename)
{
	FILE *fp;
	int count;
	int ch;

	fp = fopen(filename, "r");
	count = 0;

	if(fp == NULL)
	{
		printf("ERROR: unable to open the file: %s", filename);
		exit(1);
	}

	do {
		ch = fgetc (fp);
		if(ch == (char) '\n') count++;
	} while (ch != EOF);

	fclose(fp);
	return count;
}

int cal_char(char *filename)
{
	FILE *fp;
	int count;
	int ch;

	fp = fopen(filename, "r");
	count = 0;

	if(fp == NULL)
	{
		printf("ERROR: unable to open the file: %s", filename);
		exit(1);
	}

	do {
		ch = fgetc (fp);
		count++;
	} while (ch != EOF);

	count--; //incrementing the count even when ch == EOF
	fclose(fp);
	return count;
}

int cal_words(char *filename)
{
	FILE *fp;
	int count;
	int flag = 1;
	int ch;

	fp = fopen(filename, "r");
	count = 0;

	if(fp == NULL)
	{
		printf("ERROR: unable to open the file: %s", filename);
		exit(1);
	}

	do {
		ch = fgetc (fp);
		if((ch == ' ') || (ch == '\n') || (ch == '\t')) {
			if(flag == 0) {
				flag = 1;
				count++;
			}
		} else {
			flag = 0;
		}
	} while (ch != EOF);

	fclose(fp);
	return count;
}
