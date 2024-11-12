#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<string.h>
#include<fcntl.h>
#include<sys/sendfile.h>
#include<unistd.h>
#include "server/server.h"

#define true 1
#define BUFFER_SIZE 1024
#define error(msg) \
	do { printf(msg); exit(1); } while (0)

#define PORT 3000

int main(int argc, char* argv[]){

	struct Http_Server server;
	int init_status = init_server(&server, PORT);
	
	switch(init_status){
		case -1: error("socket"); break;
		case -2: error("bind"); break;
		default: printf("server initialized on '%d'", PORT); break;
	}

	listen(server.socket, 10);

	int client_fd = accept(server.socket, 0, 0);

	char buffer[256] = {0};
	recv(client_fd, buffer, 256, 0);

	char* f = buffer + 5;
	*strchr(f, ' ') = 0;
	
	int opened_fd = open(f, O_RDONLY);
	sendfile(client_fd, opened_fd, 0, 256);

	close(opened_fd);
	close(client_fd);
	close_server(&server);	

	return 0;
}
	
