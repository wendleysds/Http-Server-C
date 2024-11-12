#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"

int init_server(struct Http_Server* http_server, uint16_t &port){
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1)
		return -1;

	if(bind(sock_fd, &server_addr, sizeof(server_addr)) == -1)
		return -2;

	return 0;
}

int main(){return 0;}
