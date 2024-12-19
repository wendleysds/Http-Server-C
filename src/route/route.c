#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "route.h"

#define SEGMENTS_MAX 10

int split_string(char* str, const char* delim, char** output){
	int count = 0;

	char* buffer = strdup(str);
	char* token = strtok(buffer, delim);
	while(token != NULL){
		output[count++] = strdup(token);
		token = strtok(NULL, delim);
	}

	free(buffer);
	return count;
}

struct TrieNode* create_trieNode(char *segment){
	struct TrieNode* trieNode = (struct TrieNode*)malloc(sizeof(struct TrieNode));

	trieNode->segment = strdup(segment);
	trieNode->handler = NULL;
	trieNode->isDynamic = (segment[0] == ':') ? 1 : 0;	

	for(int i = 0; i < 26; i++){
		trieNode->children[i] = NULL;
	}

	return trieNode;
}

void add_route(struct TrieNode *root, char *path, struct HttpResponse (*handler)(char**)){
	char* segments[SEGMENTS_MAX];
	int segCounter = split_string(path, "/", segments);

	struct TrieNode* current = root;
	for(int i = 0; i < segCounter; i++){
		int found = 0;

		for(int j = 0; j < SEGMENTS_MAX; j++){
			if(current->children[j] && strcmp(current->children[j]->segment, segments[i]) == 0){
				current = current->children[j];
				found = 1;
				break;
			}
		}

		if(!found) {
			for(int j = 0; j < SEGMENTS_MAX; j++){
				if(!current->children[j]){
					current->children[j] = create_trieNode(segments[i]);
					current = current->children[j];
					break;
				}
			}
		}
	}

	current->handler = handler;
}

struct TrieNode* search_route(struct TrieNode *root, char *path, char **params){
	char* segments[SEGMENTS_MAX];
	int segCounter = split_string(path, "/", segments);

	struct TrieNode* current = root;
	for(int i = 0; i < segCounter; i++){
		int found = 0;

		for(int j = 0; j < SEGMENTS_MAX; j++){
			if(current->children[j] && strcmp(current->children[j]->segment, segments[i]) == 0){
				current = current->children[j];
				found = 1;
				break;
			}
			if(current->children[j] && current->children[j]->isDynamic){
				params[i] = strdup(segments[i]);
				current = current->children[j];
				found = 1;
				break;
			}
		}

		if(!found){
			return NULL;
		}
	}

	return current->handler ? current : NULL;
}

