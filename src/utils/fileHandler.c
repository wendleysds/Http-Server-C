#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "fileHandler.h"

#define FILE_ROOT "./wwwroot/"

char* file_content(char *filepath){
	char fullPath[PATH_MAX];
	char resolvedPath[PATH_MAX];
	FILE* file;
	char* content;
	size_t fileSize;

	snprintf(fullPath, sizeof(fullPath), "%s%s", FILE_ROOT, filepath);

	if(!realpath(fullPath, resolvedPath)){
		printf("resolve path error:\n%s\nfull path:\n%s\n", resolvedPath, fullPath);
		return NULL;
	}

	file = fopen(resolvedPath, "r");

	if(!file){
		printf("\nfile not found in:\n%s\n", resolvedPath);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	rewind(file);

	content = (char*)malloc(sizeof(char) * fileSize + 1);
	if(!content){
		perror("\nfile content buffer alloc failed!\n");
		return NULL;
	}

	fread(content, 1, fileSize, file);
	fclose(file);

	content[fileSize] = '\0';

	return content;
}

