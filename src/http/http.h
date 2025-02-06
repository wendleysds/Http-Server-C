#ifndef HTTP_H
#define HTTP_H

#define MAX_HEADERS 20
#define MAX_HEADERS_NAME 256
#define MAX_HEADERS_VALUE 1024

#include <inttypes.h>
#include <stdio.h>
#include "../utils/fileHandler.h"

enum ContentType{
	FILE_TYPE, // FILE* 
	STRING_TYPE, // char*
};

enum HttpMethod {
	GET = 0,
	POST,
	PUT,
	DELETE,
	PATCH,
	TRACE,
	OPTIONS,
	HEAD,
};

struct HttpBody{
	enum ContentType contentType;
	void* content;
	size_t size;
};

struct HttpHeader{
	char name[MAX_HEADERS_NAME];
	char value[MAX_HEADERS_VALUE];
};

struct HttpResponse{
	char http_version[16];
	uint16_t status_code;
	char status_message[64];
	struct HttpHeader headers[MAX_HEADERS];
	uint8_t header_count;

	struct HttpBody body;

	//Flags
	int freeBodyContent;
};

struct HttpRequest{
	char method[10];
	char path[256];
	char http_version[16];
	struct HttpHeader headers[MAX_HEADERS];
	uint8_t header_count;
	char* body;
};

void init_response(struct HttpResponse* res);
void init_request(struct HttpRequest* req);
void add_header(struct HttpHeader *headers, uint8_t* header_count, const char *name, const char *value);
char* get_header_value(struct HttpHeader *headers, uint8_t* header_count, const char* name);
enum HttpMethod valuet_method(char* str);
void parse_request(char *raw_request, struct HttpRequest* req);

#endif



