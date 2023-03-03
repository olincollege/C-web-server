#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

struct http_response{
    char* status;
    char* headers;
    char* body;
};

char* build_response(struct http_response res);
void free_response(struct http_response res);

#endif
