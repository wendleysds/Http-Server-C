#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

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

struct RouteNode* init_route(char *key, struct HttpResponse (*res)(char**)){
	struct RouteNode* instance = (struct RouteNode*)malloc(sizeof(struct RouteNode));

	if(!instance){
		printf("route malloc error");
		return NULL;
	}

	instance->key = key;
	instance->response = res;
	instance->left = instance->right = NULL;

	return instance;
}

struct RouteNode* add_route(struct RouteNode *root, char *key, struct HttpResponse (*res)(char**)){
	if(!root){
		return init_route(key, res);
	}

	

	return root;
}

struct RouteNode* search_route(struct RouteNode* root, char* key){
	if(!root){
		return NULL;
	}

	return NULL;
} 

void inorder(struct RouteNode* root){
	if(root != NULL){
		inorder(root->left);
		printf("%s -> %p \n", root->key, root->response);
		inorder(root->right);
	}
}

