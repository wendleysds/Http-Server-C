#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>

#include "route/route.h"
#include "server/httpserver.h"
#include "http/http.h"
#include "utils/fileHandler.h"

#define true 1

#define error(msg) \
	do { printf(msg); exit(1); } while (0)

#define DEFULT_PORT 3000
#define REQUEST_BUFFER 2048

#define STATIC_FILES_DIR "static/"

struct HttpResponse response_contructor(char* message, char* statusMessage, int statusCode){
	struct HttpResponse res;
	init_response(&res);

	char contentLenght[32];
	snprintf(contentLenght, sizeof(contentLenght), "%lu", strlen(message));

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	res.status_code = statusCode;
	snprintf(res.status_message, sizeof(res.status_message), "%s", statusMessage);
	add_header(res.headers, &res.header_count, "Content-Type", "text/plain");
	add_header(res.headers, &res.header_count, "Content-Length", contentLenght); 
	res.body = message;

	return res;
}

char* contentType(char* file){
	char* token = strtok(file, ".");
	char* contentType = "text/plain";

	while(token){
		if(strcmp(token, "js") == 0){
			contentType = "application/javascript";
			break;
		}
		if(strcmp(token, "css") == 0){
			contentType = "text/css";
			break;
		}
		token = strtok(NULL, ".");
	}

	return contentType;
}

struct HttpResponse get_static_file(char** params, struct HttpRequest* req){

	char filePath[(strlen(STATIC_FILES_DIR) + strlen(params[0]) + 1)];
	snprintf(filePath, sizeof(filePath), "%s%s", STATIC_FILES_DIR, params[0]);

	char* file = file_content(filePath);

	if(!file){
		return response_contructor("Not Found", "NOT FOUND", 404);
	}

	struct HttpResponse res;
	init_response(&res);

	char content_length[20];
	snprintf(content_length, sizeof(content_length), "%lu", strlen(file));

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	add_header(res.headers, &res.header_count, "Content-Type", contentType(params[0]));
	add_header(res.headers, &res.header_count, "Content-Length", content_length);
	res.dinamicAllocatedBody = true;
	res.body = file;

	return res;
}

struct HttpResponse index_page(char** params, struct HttpRequest* req){
	char* file = file_content("template/index.html");
	if(!file){
		return response_contructor("file not found", "INTERNAL SERVER ERROR", 500);
	}

	struct HttpResponse res;
	init_response(&res);

	char content_length[20];
	snprintf(content_length, sizeof(content_length), "%lu", strlen(file));

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	add_header(res.headers, &res.header_count, "Content-Type", "text/html");
	add_header(res.headers, &res.header_count, "Content-Length", content_length);
	res.dinamicAllocatedBody = true;
	res.body = file;
	
	return res;
}

struct HttpResponse hello(char** params, struct HttpRequest* req){
	struct HttpResponse res;
	init_response(&res);

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	add_header(res.headers, &res.header_count, "Content-Type", "text/plain");
	add_header(res.headers, &res.header_count, "Content-Length", "13");
	res.body = "Hello, World!";

	return res;
}

struct HttpResponse echo(char** params, struct HttpRequest* req){
	struct HttpResponse res;
	init_response(&res);

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");

	char body_buffer[1024];
	int offset = 0;

	offset += snprintf(body_buffer, sizeof(body_buffer), "%s\n", params[0]);

	add_header(res.headers, &res.header_count, "Content-Type", "text/plain");

	char content_length[16];
  snprintf(content_length, sizeof(content_length), "%d", offset - 1);

  add_header(res.headers, &res.header_count, "Content-Length", content_length);
	res.dinamicAllocatedBody = 1;
	res.body = strdup(body_buffer);

	return res;
}

struct HttpResponse post_test(char** params, struct HttpRequest* req){
	enum HttpMethod method = valuet_method(req->method);
	struct HttpResponse res;

	if(method != POST){
		res = response_contructor("Method not allowed", "METHOD NOT ALLOWED", 405);
		add_header(res.headers, &res.header_count, "Allow", "POST");
		return res;
	}

	init_response(&res);
	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");

	if(req->body){
		char content_length[20];
		snprintf(content_length, sizeof(content_length), "%lu", strlen(req->body));
		add_header(res.headers, &res.header_count, "Content-Type", "application/json");
		add_header(res.headers, &res.header_count, "Content-Length", content_length);

		res.body = req->body;

		return res;
	}

	add_header(res.headers, &res.header_count, "Content-Length", "0");

	return res;
}

int main(int argc, char* argv[]){

	int port;

	if(argc == 2){
		port = atoi(argv[1]);
		if(port == 0){
			port = DEFULT_PORT;
		}
	}
	else{
		port = DEFULT_PORT;
	}

	struct Http_Server server;
	printf("\nInitializing server...\n\n");
	int init_status = init_server(&server, port);
	
	switch(init_status){
		case -1: error("Socket error\n"); break;
		case -2: error("Bind error\n"); break;
		default: printf("Server initialized on '%d'\n", port); break;
	}

	int client_fd;
	
	int addrlen = sizeof(server.server_addr);
	
	printf("\nSetting routes...\n");
	struct TrieNode* route_root = create_trieNode("");

	add_route(route_root, "/", &index_page);
	add_route(route_root, "/echo/:message", &echo);
	add_route(route_root, "/hello", &hello);
	add_route(route_root, "/static/:file", &get_static_file);
	add_route(route_root, "/post", &post_test);

	printf("\nAllocating dynamic buffer for requests...\n");
	char* request_buffer = (char*)malloc(sizeof(char) * REQUEST_BUFFER + 1);
	if(!request_buffer){
		printf("Alloc Failed!");
		exit(1);
	}
	printf("\nBuffer size allocated: %d\n", REQUEST_BUFFER + 1);

	struct RouteNode* fidedRouteNode;
	struct HttpResponse res;
	struct HttpRequest req;
	char* params[15] = { NULL };

	printf("\nReady for connections\r\n");
	while(true){
		if((client_fd = accept(server.socket, (struct sockaddr *)&server.server_addr, (socklen_t *)&addrlen)) < 0){
			printf("\nError accpting connection\n");
			exit(1);
		}	
		
		int received = recv(client_fd, request_buffer, REQUEST_BUFFER, 0);

		if(received > REQUEST_BUFFER || received < 0){
			printf("\nInvalid request size:\n");
			res = response_contructor("Invalid request size", "INTERNAL SERVER ERROR", 500);
			goto send;
		}

		parse_request(request_buffer, &req);

		struct TrieNode* findedRoute = search_route(route_root, req.path, params);

		if(findedRoute && findedRoute->handler){
			res = findedRoute->handler(params, &req);
		}else{
			res = response_contructor("Not Found", "NOT FOUND", 404);
		}

send:

		send_response(&client_fd, &res);

		memset(request_buffer, 0, REQUEST_BUFFER + 1);

		for(int i = 0; params[i]; i++){
			free(params[i]);
			params[i] = NULL;
		}

		close(client_fd);
		printf("\n\nReady for next connection...\r\n");
	}

	close(server.socket);
	free(request_buffer);

	printf("\n\nend of program\n");
	return 0;
}

