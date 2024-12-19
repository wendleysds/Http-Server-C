#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<string.h>
#include<fcntl.h>
#include<sys/sendfile.h>
#include<unistd.h>

#include "route/route.h"
#include "server/httpserver.h"
#include "http/http.h"

#define true 1

#define error(msg) \
	do { printf(msg); exit(1); } while (0)

#define PORT 3000
#define REQUEST_BUFFER 2048

struct HttpResponse not_found(char** params){
	struct HttpResponse res;
	init_response(&res);

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	res.status_code = 404;
	snprintf(res.status_message, sizeof(res.status_message), "NOT FOUND");
	add_header(res.headers, &res.header_count, "Content-Type", "text/plain");
	add_header(res.headers, &res.header_count, "Content-Length", "9");
	res.body = "Not Found";

	return res;
}

struct HttpResponse server_error(char* message){
	struct HttpResponse res;
	init_response(&res);

	char lenght_buffer[32];
	snprintf(lenght_buffer, sizeof(lenght_buffer), "%lu", strlen(message));

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	res.status_code = 500;
	snprintf(res.status_message, sizeof(res.status_message), "INTERNAL SERVER ERROR");
	add_header(res.headers, &res.header_count, "Content-Type", "text/plain");
	add_header(res.headers, &res.header_count, "Content-Length", lenght_buffer);
	res.body = message;

	return res;
}

struct HttpResponse index_page(char** params){
	struct HttpResponse res;
	init_response(&res);

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	add_header(res.headers, &res.header_count, "Content-Type", "text/plain");
	add_header(res.headers, &res.header_count, "Content-Length", "10");
	res.body = "Index Page";

	return res;
}

struct HttpResponse hello(char** params){
	struct HttpResponse res;
	init_response(&res);

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	add_header(res.headers, &res.header_count, "Content-Type", "text/plain");
	add_header(res.headers, &res.header_count, "Content-Length", "13");
	res.body = "Hello, World!";

	return res;
}

struct HttpResponse echo(char** params){
	struct HttpResponse res;
	init_response(&res);

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");

	char body_buffer[1024];
	int offset = 0;

	for(int i = 1; params[i] != NULL; i++){
		offset += snprintf(body_buffer, sizeof(body_buffer), "%s\n", params[i]);
	}

	add_header(res.headers, &res.header_count, "Content-Type", "text/plain");

	char content_length[16];
    snprintf(content_length, sizeof(content_length), "%d", offset - 1);
    add_header(res.headers, &res.header_count, "Content-Length", content_length);

	res.body = strdup(body_buffer);

	return res;
}

int main(int argc, char* argv[]){
	struct Http_Server server;
	printf("\nInitializing server\n\n");
	int init_status = init_server(&server, PORT);
	
	switch(init_status){
		case -1: error("Socket error\n"); break;
		case -2: error("Bind error\n"); break;
		default: printf("Server initialized on '%d'\n", PORT); break;
	}

	int client_fd;
	
	int addrlen = sizeof(server.server_addr);
	struct HttpRequest req;

	printf("\nSetting routes\n");
	struct TrieNode* route_root = create_trieNode("");

	add_route(route_root, "/echo/:message", &echo);
	add_route(route_root, "/hello", &hello);

	printf("\nAllocating dynamic buffer for requests:\nbuffer size: %d\n", REQUEST_BUFFER + 1);
	char* request_buffer = (char*)malloc(sizeof(char) * REQUEST_BUFFER + 1);
	if(!request_buffer){
		printf("Alloc Failed!");
		exit(1);
	}

	struct RouteNode* fidedRouteNode;
	struct HttpResponse res;

	printf("\nReady for connections\r\n\n");
	while(true){
		if((client_fd = accept(server.socket, (struct sockaddr *)&server.server_addr, (socklen_t *)&addrlen)) < 0){
			printf("\nError accpting connection\n");
			exit(1);
		}	
		
		int received = recv(client_fd, request_buffer, REQUEST_BUFFER, 0);

		if(received > REQUEST_BUFFER || received < 0){
			printf("\nInvalid request size:\n");
			res = server_error("Invalid request size");
			goto send;
		}

		parse_request(request_buffer, &req);

		char* params[10] = { NULL };
		struct TrieNode* findedRoute = search_route(route_root, req.path, params);

		if(findedRoute && findedRoute->handler){
			res = findedRoute->handler(params);
		}else{
			res = not_found(NULL);
		}

send:

		send_response(&client_fd, &res);

		memset(request_buffer, 0, REQUEST_BUFFER + 1);
		close(client_fd);
		printf("\n\nReady for next connection\r\n");
	}

	close(server.socket);
	free(request_buffer);

	printf("\n\nend of program\n");
	return 0;
}
	
