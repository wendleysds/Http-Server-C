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
#define BUFFER_SIZE 1024
#define RESPONSE_BUFFER_SIZE 4096
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
	size_t buffer_sizer = 0;

	buffer_sizer += strlen(res->http_version) + 1;
	buffer_sizer += 3 + 1;
	buffer_sizer += strlen(res->status_message) + 2;
	
	uint8_t i;
	for(i = 0; i < res->header_count; i++){
		buffer_sizer += (strlen(res->headers[i].name) + 2 + strlen(res->headers[i].value) + 2);
	}
	buffer_sizer += 2;

	/*if(res->body){
		buffer_sizer += strlen(res->body);
	}*/

	char* response_buffer = (char*)malloc(sizeof(char) * buffer_sizer + 1);

	if(!response_buffer){
		printf("\nmalloc failed for 'response_buffer' in httpserver.c\n");
		char server_error[64];
		snprintf(server_error, sizeof(server_error), "%s %d %s\r\n", res->http_version, 500, "INTERNAL SERVER ERROR");
		send(*client_fd, server_error, sizeof(server_error), MSG_NOSIGNAL);
		return;
	}

	uint16_t offset = 0;

	/*
	 * httpVersion status status_message\r\n
	 * header_name: header_value\r\n
	 * header_name: header_value\r\n
	 * ... headers count
	 * header_name: header_value\r\n
	 * \r\n
	 * body
	 * */

	offset += snprintf(response_buffer + offset, buffer_sizer - offset + 1, "%s %d %s\r\n", res->http_version, res->status_code, res->status_message);

	for(i = 0; i < res->header_count; i++){
		offset += snprintf(response_buffer + offset, buffer_sizer - offset + 1, "%s: %s\r\n", res->headers[i].name, res->headers[i].value);
	}

	offset += snprintf(response_buffer + offset, buffer_sizer - offset + 1, "\r\n");

	/*if(res->body){
		offset += snprintf(response_buffer + offset, buffer_sizer - offset + 1, "%s", res->body);
	}*/
	
	//write(*client_fd, response_buffer, strlen(response_buffer));
	send(*client_fd, response_buffer, strlen(response_buffer), MSG_NOSIGNAL);

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

	free(response_buffer);
	response_buffer = NULL;

	if(res->freeBodyContent){
		free(res->body.content);
		res->body.content = NULL;
	}
}

