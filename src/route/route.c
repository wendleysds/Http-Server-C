#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "route.h"

struct RouteNode* init_route(char *key, struct HttpResponse (*res)()){
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
struct RouteNode* add_route(struct RouteNode *root, char *key, struct HttpResponse (*res)()){
	if(!root){
		return init_route(key, res);
	}

	int r = strcmp(key, root->key);

	if(r == 0){
		printf("\nA Route for \"%s\" alery exists\n", key);
	} else if(r > 0){
		root->right = add_route(root->right, key, res);
	}else{
		root->left = add_route(root->left, key, res);
	}
	
	return NULL;
}

struct RouteNode* search_route(struct RouteNode* root, char* key){
	if(!root){
		return NULL;
	}

	int r = strcmp(key, root->key);

	if(r == 0){
		return root;
	} else if(r > 0){
		return search_route(root->right, key);
	} else{
		return search_route(root->left, key);
	}
} 

void inorder(struct RouteNode* root){
	if(root != NULL){
		inorder(root->left);
		printf("%s -> %p \n", root->key, root->response);
		inorder(root->right);
	}
}

