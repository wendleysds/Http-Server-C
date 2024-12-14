#ifndef HTTP_ROUTES_H
#define HTTP_ROUTES_H

#include "../http/http.h"

struct RouteNode{
	char* key;
	struct HttpResponse (*response)(char**);

	struct RouteNode *left, *right;
};

struct RouteNode* init_route(char* key, struct HttpResponse (*res)(char**));
struct RouteNode* add_route(struct RouteNode* root, char* key, struct HttpResponse (*res)(char**));
struct RouteNode* search_route(struct RouteNode* root, char* key); 

void inorder(struct RouteNode* root);

#endif
