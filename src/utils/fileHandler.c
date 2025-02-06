#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "fileHandler.h"

#define FILE_ROOT "./wwwroot/"

struct AssetFile* init_assetfile(){
	struct AssetFile* assetFile = (struct AssetFile*)malloc(sizeof(struct AssetFile));
	assetFile->file = NULL;
	assetFile->size = 0;
	return assetFile;
}

struct AssetFile* get_file(char *filepath, const char* mode){
	char fullPath[PATH_MAX];
	char resolvedPath[PATH_MAX];
	FILE* file;
	size_t fileSize;

	snprintf(fullPath, sizeof(fullPath), "%s%s", FILE_ROOT, filepath);

	if(!realpath(fullPath, resolvedPath)){
		printf("resolve path error:\n%s\nfull path:\n%s\n", resolvedPath, fullPath);
		return NULL;
	}
	
	file = fopen(resolvedPath, mode);

	if(!file){
		printf("\nfile not found in:\n%s\n", resolvedPath);
		return NULL;
	}

	struct AssetFile* assetFile = init_assetfile();

	if(!assetFile){
		perror("Asset File malloc failed!");
		fclose(file);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	rewind(file);

	assetFile->size = fileSize;
	assetFile->file = file;

	return assetFile;
}

