#include<stdlib.h>
#include<string.h>

#include "hashMap.h"
#include "linkList.h"

int hash_function(struct HashMap* mp, char* key){
	int bucketIndex;
	int sum = 0, factor = 3;
	
	for(int i = 0; i < strlen(key); i++){
		sum = ((sum % mp->capacity) + (((int)key[i]) * factor) % mp->capacity) % mp->capacity;
		factor = ((factor % __INT16_MAX__) * (31 % __INT16_MAX__)) % __INT16_MAX__;
	}

	bucketIndex = sum;
	return bucketIndex;
}

void init_hashMap(struct HashMap *mp, int capacity){
	mp->capacity = capacity;
	mp->numOfElements = 0;

	mp->arr = (struct Node**)malloc(sizeof(struct Node*) * capacity);
}

void insert_hashMap(struct HashMap *mp, char *key, char *value){
	int bucketIndex = hash_function(mp, key);
	struct Node* node = (struct Node*)malloc(sizeof(struct Node));

	init_Node(node, key, value);

	if(mp->arr[bucketIndex] == NULL){
		mp->arr[bucketIndex] = node;
	}else{
		node->next = mp->arr[bucketIndex];
		mp->arr[bucketIndex] = node;
	}
}

void delete_hashMap(struct HashMap *mp, char *key){
	int bucketIndex = hash_function(mp, key);

	struct Node* prevNode = NULL;
	struct Node* currNode = mp->arr[bucketIndex];

	while(currNode != NULL){
		if(strcmp(key, currNode->key)){
			if(currNode == mp->arr[bucketIndex]){
				mp->arr[bucketIndex] = currNode->next;
			}else{
				prevNode->next = currNode->next;
			}

			free(currNode);
			break;
		}

		prevNode = currNode;
		currNode = currNode->next;
	}
}

char* search_hashMap(struct HashMap *mp, char *key){
	int bucketIndex = hash_function(mp, key);

	struct Node* buckedHead = mp->arr[bucketIndex];

	while(buckedHead != NULL){
		if(buckedHead->key == key){
			return buckedHead->value;
		}
		buckedHead = buckedHead->next;
	}

	return NULL;
}

