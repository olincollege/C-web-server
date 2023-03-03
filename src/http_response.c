#include "http_response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *build_response(struct http_response res) {
    char *response_str;
    // Compute the length of the response string
    int length = strlen(res.status) + strlen(res.headers) + strlen(res.body) + 6;
    // Allocate memory for the response string
    response_str = malloc(length);
    // Build the response string
    sprintf(response_str, "HTTP/1.1 %s\n%s\n%s", res.status, res.headers, res.body);
    return response_str;
}

void free_response(struct http_response res) {
    free(res.status);
    free(res.headers);
    free(res.body);
}
