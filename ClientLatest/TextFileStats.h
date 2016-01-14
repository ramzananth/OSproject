/*
 * TextFileStats.h
 *
 *  Created on: Nov 16, 2013
 *      Author: ramyaananth
 */

#pragma once
#include "header.h"

void countCharWordsSentCase(char* fileName, char* fileName2, int* charCount, int* wordCount, int* sentenceCount);

int compareFiles(const char* fileName, const char* fileName2);

void cal_word_info(char *filename, struct word* cWI);

void cal_char_info(char *filename, int* charCount);

