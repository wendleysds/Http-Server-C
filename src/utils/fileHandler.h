#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <stdio.h>

struct AssetFile{
	FILE* file;
	unsigned long size;
};

struct AssetFile* init_assetfile();
struct AssetFile* get_file(char* path, const char* mode);

#endif
