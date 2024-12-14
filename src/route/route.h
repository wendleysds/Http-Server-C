#ifndef HTTP_ROUTES_H
#define HTTP_ROUTES_H

#include "../http/http.h"

struct TrieNode{
	char* segment;
	struct TrieNode *children[30];
	int is_route;
	struct HttpResponse (*handler)(char**);

	struct TrieNode* wildCard; //for dynamic (:);
};

struct TrieNode* create_node(char* segment);
void add_route(struct TrieNode* root, char* path, struct HttpResponse (*handler)(char**));
struct TrieNode* search_route(struct TrieNode* root, char* path, char** params);

#endif
