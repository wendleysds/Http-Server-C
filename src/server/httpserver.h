#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <inttypes.h>
#include <netinet/in.h>
#include "../http/http.h"

struct Http_Server{
	uint16_t port;
	int socket;
	struct sockaddr_in server_addr;
};

int init_server(struct Http_Server*, uint16_t port);
void close_server(struct Http_Server*);

void send_response(int* client_fd, struct HttpResponse* res);

#endif
