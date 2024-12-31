#ifndef HTTP_ROUTES_H
#define HTTP_ROUTES_H

#include "../http/http.h"

struct TrieNode{
	char* segment;
	struct TrieNode *children[26];
	struct HttpResponse (*handler)(char**, struct HttpRequest*);
	int isDynamic;
};

struct TrieNode* create_trieNode(char* segment);
void add_route(struct TrieNode* root, char* path, struct HttpResponse (*handler)(char**, struct HttpRequest*));
struct TrieNode* search_route(struct TrieNode* root, char* path, char** params);

#endif
