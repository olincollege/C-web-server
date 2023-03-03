#include "http_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct http_request parse_request(char *request_str) {
    struct http_request req;
    char *line;
    int i = 0;
    line = strtok(request_str, " \n\n");// Split by newline and space(" ") delimeter
    req.method = strdup(line);
    line = strtok(NULL, " \n\n");// get next pointer in split string
    if (line) {
        // Check if request path contains query string
        char *query_start = strchr(line, '?');
        if (query_start) {
            req.path = strndup(line, query_start - line);
            req.query_string = strdup(query_start + 1);
        } else {
            req.path = strdup(line);
            req.query_string = NULL;
        }
    }
    // Parse request headers
    req.headers = NULL;
    while ((line = strtok(NULL, "\n\n")) && *line != '\0') {
        if (i == 0) {
            req.headers = strdup(line);
        } else {
            req.headers = realloc(req.headers, strlen(req.headers) + strlen(line) + 2);
            strcat(req.headers, "\n");
            strcat(req.headers, line);
        }
        i++;
    }
    // Parse request body (if any) and copy everything to req.body
    req.body = NULL;
    if (line && *(line + 1) != '\0') {
        req.body = strdup(line + 1);
    }
    return req;
}

void free_request(struct http_request req) {
    free(req.method);
    free(req.path);
    free(req.query_string);
    free(req.headers);
    free(req.body);
}
