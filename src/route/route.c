#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "route.h"

#define MAX_SEGMENTS 10

int split_string(char* str, const char* delim, char** buff){
	int count = 0;

	char* token = strtok(str, delim);
	while(token != NULL){
		buff[count++] = token;
		token = strtok(NULL, delim);
	}

	return count;

	
}

struct TrieNode* create_node(char *segment){
	struct TrieNode* node = (struct TrieNode*)malloc(sizeof(struct TrieNode));

	node->segment = segment;
	node->is_route = 0;
	node->handler = NULL;
	node->wildCard = NULL;

	for(int i = 0; i < 30; i++){
		node->children[i] = NULL;
	}

	return node;
}

void add_route(struct TrieNode *root, char *path, struct HttpResponse (*handler)(char**)){
	char* segments[10];


}

struct TrieNode* search_route(struct TrieNode *root, char *path, char **params){
	return NULL;
}

