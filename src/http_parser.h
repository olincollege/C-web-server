#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

struct  http_request{
    char* method;
    char* path;
    char* query_string;
    char* headers;
    char* body;
};

struct http_request parse_request(char* request_str);
void free_request(struct http_request req);

#endif /* HTTP_PARSER_H */
