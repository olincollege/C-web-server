#ifndef C_WEB_SERVER_CONNECTION_H
#define C_WEB_SERVER_CONNECTION_H

#include <arpa/inet.h>

char *get_status_code_string(int status_code);
char *get_content_type_from_ext(char *ext);

void handle_connection(int sockfd);

#endif//C_WEB_SERVER_CONNECTION_H
