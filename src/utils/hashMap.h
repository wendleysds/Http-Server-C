#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "linkList.h"

struct HashMap{
	int numOfElements, capacity;

	struct Node** arr;
};

void init_hashMap(struct HashMap* mp, int capacity);
void insert_hashMap(struct HashMap* mp, char* key, char* value);
void delete_hashMap(struct HashMap* mp, char* key);
char* search_hashMap(struct HashMap* mp, char* key);

#endif
