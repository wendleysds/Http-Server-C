#include <stdlib.h>
#include "linkList.h"

void init_Node(struct Node* node, char* key, char* value){
    node->key = key;
    node->value = value;
    node->next = NULL;
}