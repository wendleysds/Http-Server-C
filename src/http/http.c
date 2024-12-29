#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "http.h"

void init_response(struct HttpResponse* res){
	res->http_version[0] = '\0';
	res->status_code = 200;
	snprintf(res->status_message, sizeof(res->status_message), "OK");
	res->header_count = 0;
	res->dinamicAllocatedBody = 0;
	res->body = NULL;
}

void init_request(struct HttpRequest *req){
	req->method[0] = '\0';
	req->path[0] = '\0';
	req->http_version[0] = '\0';
	req->header_count = 0;
	req->body = NULL;
}

void add_header(struct HttpHeader *headers, uint8_t* header_count, const char *name, const char *value){
	if(*header_count < MAX_HEADERS){
		snprintf(headers[*header_count].name, MAX_HEADERS_NAME, "%s", name);
		snprintf(headers[*header_count].value, MAX_HEADERS_VALUE, "%s", value);
		(*header_count)++;
	}
}

void parse_request(char *raw_request, struct HttpRequest *req){
	init_request(req);
	char* line = strtok(raw_request, "\r\n");
	sscanf(line, "%s %s %s", req->method, req->path, req->http_version);
	while((line = strtok(NULL, "\r\n")) && strlen(line) > 0){
		char header_name[MAX_HEADERS_NAME], header_value[MAX_HEADERS_VALUE];

		sscanf(line, "%[^:]: %[^\r\n]", header_name, header_value);
		add_header(req->headers, &req->header_count, header_name, header_value);
	}

	line = strtok(NULL, "");
	if(line){
		req->body = strdup(line);
	}
}

