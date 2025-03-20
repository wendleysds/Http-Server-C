#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include "httpserver.h"

#define BACKLOG_MAX 10
#define RESPONSE_BUFFER_SIZE 1024
#define CHUNK_SIZE 4096

void close_server(struct Http_Server* http_server){
	close(http_server->socket);
	free(http_server);
}

int init_server(struct Http_Server* http_server, uint16_t port){
	http_server->port = port;
	http_server->socket = socket(AF_INET, SOCK_STREAM, 0);	

	memset(&http_server->server_addr, 0, sizeof(http_server->server_addr));

	http_server->server_addr.sin_family = AF_INET;
	http_server->server_addr.sin_port = htons(port);
	http_server->server_addr.sin_addr.s_addr = INADDR_ANY;
	

	if(http_server->socket == -1)
		return -1;

	if(bind(http_server->socket, (struct sockaddr*) &http_server->server_addr, sizeof(http_server->server_addr)) == -1)
		return -2;

	listen(http_server->socket, BACKLOG_MAX);

	return 0;
}

void send_response(int *client_fd, struct HttpResponse *res){
	char response_buffer[RESPONSE_BUFFER_SIZE];
	
	uint8_t statusLine = snprintf(response_buffer, RESPONSE_BUFFER_SIZE, "%s %d %s\r\n", res->http_version, res->status_code, res->status_message);

	size_t bytesWrited = statusLine;
	for(int i = 0; i < res->header_count; i++){
		int headerSize = strlen(res->headers[i].name) + 2 + strlen(res->headers[i].value) + 2;

		if(bytesWrited + headerSize + 2 >= RESPONSE_BUFFER_SIZE){
			send(*client_fd, response_buffer, bytesWrited, MSG_NOSIGNAL);
			bytesWrited = 0;
			i--;

			continue;
		}

		bytesWrited += snprintf(response_buffer + bytesWrited, RESPONSE_BUFFER_SIZE - bytesWrited + 1, "%s: %s\r\n", res->headers[i].name, res->headers[i].value);
	}

	bytesWrited += snprintf(response_buffer + bytesWrited, RESPONSE_BUFFER_SIZE - bytesWrited + 1, "\r\n");

	send(*client_fd, response_buffer, bytesWrited, MSG_NOSIGNAL);

	if(res->body.content){
		char buffer[CHUNK_SIZE];
		size_t bytesRead;

		if(res->body.contentType == FILE_TYPE){
			while((bytesRead = fread(buffer, 1, CHUNK_SIZE, (FILE*)res->body.content)) > 0){
				if(send(*client_fd, buffer, bytesRead, MSG_NOSIGNAL) == -1){
					perror("Erro ao enviar arquivo");
					fclose((FILE*)res->body.content);
					break;
				}
			}

			fclose((FILE*)res->body.content);
		}
		else{
			send(*client_fd, (char*)res->body.content, res->body.size, MSG_NOSIGNAL);
		}
	}

	if(res->freeBodyContent){
		free(res->body.content);
		res->body.content = NULL;
	}
}

