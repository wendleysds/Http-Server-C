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

#define FILES_ROOT "../wwwroot"
#define FILES_TEMPLATE "../wwwroot/template"
#define FILES_STATIC "../wwwroot/static"

struct HttpResponse not_found(){
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

struct HttpResponse index_page(){
	struct HttpResponse res;
	init_response(&res);

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	add_header(res.headers, &res.header_count, "Content-Type", "text/plain");
	add_header(res.headers, &res.header_count, "Content-Length", "10");
	res.body = "Index Page";

	return res;
}

struct HttpResponse hello(){
	struct HttpResponse res;
	init_response(&res);

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	add_header(res.headers, &res.header_count, "Content-Type", "text/plain");
	add_header(res.headers, &res.header_count, "Content-Length", "13");
	res.body = "Hello, World!";

	return res;
}

int main(int argc, char* argv[]){
	struct Http_Server server;
	printf("\nInitializing server\n\n");
	int init_status = init_server(&server, PORT);
	
	switch(init_status){
		case -1: error("socket\n"); break;
		case -2: error("bind\n"); break;
		default: printf("server initialized on '%d'\n", PORT); break;
	}

	int client_fd;
	
	int addrlen = sizeof(server.server_addr);
	struct HttpRequest req;

	printf("\nSetting routes\n");
	struct RouteNode* routeRoot = init_route("/", &index_page);
	add_route(routeRoot, "/hello", &hello);


	printf("\nInorder:\n");
	inorder(routeRoot);

	printf("\nAlloc dynamic request buffer:\nbuffer size: %d\n", REQUEST_BUFFER + 1);
	char* request_buffer = (char*)malloc(sizeof(char) * REQUEST_BUFFER + 1);
	if(!request_buffer){
		printf("Alloc Failed!");
		exit(1);
	}

	printf("\nReady for connections\r\n\n");
	while(true){
		if((client_fd = accept(server.socket, (struct sockaddr *)&server.server_addr, (socklen_t *)&addrlen)) < 0){
			printf("\nError accpting connection\n");
			exit(1);
		}	
		
		recv(client_fd, request_buffer, REQUEST_BUFFER, 0);

		parse_request(request_buffer, &req);

		struct RouteNode* fidedRouteNode = search_route(routeRoot, req.path);

		struct HttpResponse res;

		if(!fidedRouteNode){
			res = not_found();
		}else{
			res = fidedRouteNode->response();
		}

		send_response(&client_fd, &res);

		memset(request_buffer, 0, REQUEST_BUFFER + 1);
		close(client_fd);
		printf("\n\nReady for next connection\r\n");
	}

	close(server.socket);

	printf("\n\nend of program\n");
	return 0;
}
	
