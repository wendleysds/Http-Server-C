#ifndef LINK_LISTS_H
#define LINK_LISTS_H

struct Node{
	char* key;
	char* value;
	struct Node* next;
};

void init_Node(struct Node* node, char* key, char* value);

#endif
