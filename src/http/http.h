#ifndef HTTP_H
#define HTTP_H

#define MAX_HEADERS 20
#define MAX_HEADERS_NAME 256
#define MAX_HEADERS_VALUE 1024

#include <inttypes.h>

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
	int dinamicAllocatedBody;
	char* body;
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
enum HttpMethod valuet_method(char* str);
void parse_request(char *raw_request, struct HttpRequest* req);

#endif



