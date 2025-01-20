#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <signal.h>

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

char* contentType(char* file){
	char* token = strtok(file, ".");
	char* contentType = "text/plain";

	while(token){
		if(strcmp(token, "js") == 0){
			contentType = "application/javascript";
		}
		if(strcmp(token, "css") == 0){
			contentType = "text/css";
		}
		if(strcmp(token, "html") == 0 || strcmp(token, "htm") == 0){
			contentType = "text/html";
		}
		token = strtok(NULL, ".");
	}

	return contentType;
}

struct HttpResponse simple_response(char* message, char* statusMessage, int statusCode){
	struct HttpResponse res;
	init_response(&res);

	char contentLenght[32];
	if(message){
		snprintf(contentLenght, sizeof(contentLenght), "%lu", strlen(message));
	}else{
		snprintf(contentLenght, sizeof(contentLenght), "0");
	}

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	res.status_code = statusCode;
	snprintf(res.status_message, sizeof(res.status_message), "%s", statusMessage);
	add_header(res.headers, &res.header_count, "Content-Type", "text/plain");
	add_header(res.headers, &res.header_count, "Content-Length", contentLenght); 
	res.body = message;

	return res;
}

struct HttpResponse file_response(char* filePath){
	char* file = file_content(filePath);
	if(!file){
		return simple_response(NULL, "NOT FOUND", 404);
	}

	char* fileName = strdup(filePath);

	struct HttpResponse res;
	init_response(&res);

	char contentLength[20];
	snprintf(contentLength, sizeof(contentLength), "%lu", strlen(file));

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	add_header(res.headers, &res.header_count, "Content-Type", contentType(fileName));
	add_header(res.headers, &res.header_count, "Content-Length", contentLength);
	res.dinamicAllocatedBody = true;
	res.body = file;
	
	free(fileName);
	fileName = NULL;

	return res;
}

struct HttpResponse get_static_file(char** params, struct HttpRequest* req){
	char filePath[(strlen(STATIC_FILES_DIR) + (strlen(params[0]) + 1))];
	snprintf(filePath, sizeof(filePath), "%s%s", STATIC_FILES_DIR, params[0]);

	return file_response(filePath);
}

struct HttpResponse index_page(char** params, struct HttpRequest* req){
	char* file = file_content("template/index.html");
	if(!file){
		return simple_response("file not found", "INTERNAL SERVER ERROR", 500);
	}

	struct HttpResponse res;
	init_response(&res);

	char contentLength[20];
	snprintf(contentLength, sizeof(contentLength), "%lu", strlen(file));

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	add_header(res.headers, &res.header_count, "Content-Type", "text/html");
	add_header(res.headers, &res.header_count, "Content-Length", contentLength);
	res.dinamicAllocatedBody = true;
	res.body = file;
	
	return res;
}

struct HttpResponse hello_page(char** params, struct HttpRequest* req){
	char* file = file_content("template/hello.html");
	if(!file){
		return simple_response("file not found", "INTERNAL SERVER ERROR", 500);
	}

	struct HttpResponse res;
	init_response(&res);

	char contentLenght[20];
	snprintf(contentLenght, sizeof(contentLenght), "%lu", strlen(file));

	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");
	add_header(res.headers, &res.header_count, "Content-Type", "text/html");
	add_header(res.headers, &res.header_count, "Content-Length", contentLenght);
	res.dinamicAllocatedBody = true;
	res.body = file;

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

	char contentLength[32];
  snprintf(contentLength, sizeof(contentLength), "%d", offset - 1);

  add_header(res.headers, &res.header_count, "Content-Length", contentLength);
	res.dinamicAllocatedBody = 1;
	res.body = strdup(body_buffer);

	return res;
}

struct HttpResponse post(char** params, struct HttpRequest* req){
	enum HttpMethod method = valuet_method(req->method);
	struct HttpResponse res;

	if(method != POST){
		res = simple_response("Method not allowed", "METHOD NOT ALLOWED", 405);
		add_header(res.headers, &res.header_count, "Allow", "POST");
		return res;
	}

	init_response(&res);
	snprintf(res.http_version, sizeof(res.http_version), "HTTP/1.1");

	if(req->body){
		char contentLength[20];
		snprintf(contentLength, sizeof(contentLength), "%lu", strlen(req->body));
		add_header(res.headers, &res.header_count, "Content-Type", "application/json");
		add_header(res.headers, &res.header_count, "Content-Length", contentLength);

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

	//ignore sigchild
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = SA_RESTART | SA_NOCLDWAIT;
	sigaction(SIGCHLD, &sa, NULL);

	int client_fd;
	
	int addrlen = sizeof(server.server_addr);
	
	printf("\nSetting routes...\n");
	struct TrieNode* route_root = create_trieNode("");

	add_route(route_root, "/", &index_page);
	add_route(route_root, "/hello", &hello_page);
	add_route(route_root, "/post", &post);
	add_route(route_root, "/echo/:message", &echo);
	add_route(route_root, "/static/:file", &get_static_file);
	
	printf("\nAllocating dynamic buffer for requests...\n");
	char* request_buffer = (char*)malloc(sizeof(char) * REQUEST_BUFFER + 1);
	if(!request_buffer){
		printf("\nAlloc Failed!\n");
		exit(1);
	}
	printf("Buffer size allocated: %d\n", REQUEST_BUFFER + 1);

	printf("\nReady for connections\r\n");
	while(true){
		if((client_fd = accept(server.socket, (struct sockaddr *)&server.server_addr, (socklen_t *)&addrlen)) < 0){
			printf("\nError accpting connection\n");
			exit(1);
		}	

		pid_t pid = fork();

		if(pid < 0){
			perror("Fork failed");
			close(client_fd);
			continue;
		}

		if(pid == 0){
			close(server.socket);

			int received = recv(client_fd, request_buffer, REQUEST_BUFFER, 0);
			
			struct HttpResponse res;

			if(received > REQUEST_BUFFER || received < 0){
				perror("\nInvalid request size:\n");
				res = simple_response("Invalid request size", "INTERNAL SERVER ERROR", 500);
				send_response(&client_fd, &res);
				exit(0);
			}

			struct HttpRequest req;
			char* params[10] = { NULL };

			parse_request(request_buffer, &req);

			struct TrieNode* findedRoute = search_route(route_root, req.path, params);

			if(findedRoute && findedRoute->handler){
				res = findedRoute->handler(params, &req);
			}else{
				res = file_response("template/404.html");
			}

			send_response(&client_fd, &res);

			memset(request_buffer, 0, REQUEST_BUFFER + 1);

			for(int i = 0; params[i]; i++){
				free(params[1]);
				params[i] = NULL;
			}

			close(client_fd);
			printf("\nHandled request in child process %d\n", getpid());

			exit(0);
		}
		else{
			close(client_fd);
		}
	}

	close(server.socket);
	free(request_buffer);

	printf("\n\nend of program\n");
	return 0;
}

