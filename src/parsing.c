#include "parsing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct http_request* new_request() {
    struct http_request *req = malloc(sizeof(struct http_request));
    req->method = NULL;
    req->path = NULL;
    req->query_string = NULL;
    req->headers = NULL;
    req->body = NULL;
    return req;
}

struct http_request *parse_request(char *request_str) {
    struct http_request *req = new_request();
    char *line;
    int i = 0;
    line = strtok(request_str, " \n\n");// Split by newline and space(" ") delimeter
    req->method = strdup(line);
    line = strtok(NULL, " \n\n");// get next pointer in split string
    if (line) {
        // Check if request path contains query string
        char *query_start = strchr(line, '?');
        if (query_start) {
            req->path = strndup(line, query_start - line);
            req->query_string = strdup(query_start + 1);
        } else {
            req->path = strdup(line);
            req->query_string = NULL;
        }
    }
    // Parse request headers
    req->headers = NULL;
    while ((line = strtok(NULL, "\n\n")) && *line != '\0') {
        if (i > 0) {
            size_t offset = req->headers == NULL ? 0 : strlen(req->headers);
            req->headers = realloc(req->headers, offset + strlen(line) + 2);
            strcat(req->headers, "\n");
            strcat(req->headers, line);
        }
        i++;
    }
    // Parse request body (if any) and copy everything to req.body
    req->body = NULL;
    if (line && *(line + 1) != '\0') {
        req->body = strdup(line + 1);
    }
    return req;
}

void print_request(struct http_request *req) {
    printf("\"%s\" request to \"%s\" qs \"%s\" with headers:\n\"%s\"\nand body:\n\"%s\"\n",
           req->method, req->path, req->query_string, req->headers, req->body);
}

void free_request(struct http_request *req) {
    free(req->method);
    free(req->path);
    free(req->query_string);
    free(req->headers);
    free(req->body);
}

// response

void add_header_to_response(struct http_response *res, char *header_name, char *header_value) {
    size_t name_len = strlen(header_name);
    size_t value_len = strlen(header_value);

    if (res->headers == NULL) {
        size_t new_buffer_size = name_len + value_len + 4;
        res->headers = malloc(new_buffer_size);
        res->header_size = new_buffer_size;

        sprintf(res->headers, "%s: %s", header_name, header_value);
    } else {
        size_t new_buffer_size = res->header_size + name_len + value_len + 5;
        char* new_headers = realloc(res->headers, new_buffer_size);
        res->headers = new_headers;
        res->header_size = new_buffer_size;

        strcat(res->headers, "\n");
        strcat(res->headers, header_name);
        strcat(res->headers, ": ");
        strcat(res->headers, header_value);
    }
}

void add_static_status_to_response(struct http_response *res, char *status) {
    res->status = malloc(strlen(status) + 1); // +1 for null terminator
    if (res->status != NULL) {
        strncpy(res->status, status, strlen(status));
        res->status[strlen(status)] = '\0'; // add null terminator
    }
}

void add_static_body_to_response(struct http_response *res, char *body) {
    res->body_size = strlen(body);
    res->body = malloc(strlen(body) + 1); // +1 for null terminator
    if (res->body != NULL) {
        strncpy(res->body, body, strlen(body));
        res->body[strlen(body)] = '\0'; // add null terminator
    }
}

void write_content_length_to_response(struct http_response *res) {
    char content_length_str[20];
    sprintf(content_length_str, "%zu", res->body_size);
    add_header_to_response(res, "Content-Length", content_length_str);
}

size_t get_serialized_response_buffer_size(struct http_response *res) {
    // HTTP 1.1 status line + headers + body + 2 newlines + null terminator
    return strlen(res->status) + res->header_size + res->body_size + 13;
}

size_t serialize_response(struct http_response *res, char *response_buf) {
    size_t bytes_written = sprintf(response_buf, "HTTP/1.1 %s\n%s\n\n%s", res->status, res->headers, res->body);
    return bytes_written;
}

void free_response(struct http_response *response) {
    if (response != NULL) {
        if (response->status != NULL) {
            free(response->status);
        }
        if (response->headers != NULL) {
            free(response->headers);
        }
        if (response->body != NULL) {
            free(response->body);
        }
        free(response);
    }
}


struct http_response * new_response() {
    struct http_response *res = malloc(sizeof(struct http_response));
    res->status = NULL;
    res->headers = NULL;
    res->body = NULL;
    res->header_size = 0;
    res->body_size = 0;
    return res;
}
