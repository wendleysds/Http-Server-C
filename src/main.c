#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<string.h>
#include<fcntl.h>
#include<sys/sendfile.h>
#include<unistd.h>
#include "server/httpserver.h"
#include "http/http.h"

#define true 1

#define error(msg) \
	do { printf(msg); exit(1); } while (0)

#define PORT 3000

#define FILES_ROOT "../wwwroot"
#define FILES_TEMPLATE "../wwwroot/template"
#define FILES_STATIC "../wwwroot/static"

int main(int argc, char* argv[]){
	struct Http_Server server;
	int init_status = init_server(&server, PORT);
	
	switch(init_status){
		case -1: error("socket\n"); break;
		case -2: error("bind\n"); break;
		default: printf("server initialized on '%d'\n", PORT); break;
	}

	int client_fd = accept(server.socket, 0, 0);

	char buffer[1024] = {0};
	recv(client_fd, buffer, 256, 0);

	printf("\nrequest\n");
	printf("%s", buffer);

	struct HttpResponse res;
	init_response(&res);

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	res.status_code = 200;
	snprintf(res.status_message, sizeof(res.status_message), "OK");
	add_header(res.headers, &res.header_count, "Content-Type", "text/plain");
	add_header(res.headers, &res.header_count, "Content-Length", "12");
	add_header(res.headers, &res.header_count, "Connection", "close");
	res.body = "Hello, World!";

	send_response(&client_fd, &res);

	close(client_fd);
	close(server.socket);

	printf("\n\nend of program\n");
	return 0;
}
	
