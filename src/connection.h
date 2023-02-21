#ifndef C_WEB_SERVER_CONNECTION_H
#define C_WEB_SERVER_CONNECTION_H

#include <arpa/inet.h>

struct HTTPResponse {
    int status;
    char *content_type;
    char *content;
};


struct HTTPRequest {
    char *method;
    char *path;
    char *body;
};

int serialize_response(struct HTTPResponse *response, char *buffer);

char *get_status_code_string(int status_code);

void handle_connection(int sockfd, struct sockaddr_in *client_addr);

#endif//C_WEB_SERVER_CONNECTION_H
