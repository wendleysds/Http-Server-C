#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <inttypes.h>

struct Http_Server{
	uint16_t port;
	int socket;
};

void close_server(struct Http_Server*);
int init_server(struct Http_Server*, uint16_t);

#endif
