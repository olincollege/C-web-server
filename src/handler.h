#ifndef C_WEB_SERVER_HANDLER_H
#define C_WEB_SERVER_HANDLER_H
#include "parsing.h"

int handle_request(struct http_request *request, struct http_response *response);

#endif//C_WEB_SERVER_HANDLER_H
