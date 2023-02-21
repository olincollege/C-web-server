#include "connection.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

char *get_status_code_string(int status_code) {
    switch (status_code) {
        case 200:
            return "OK";
        case 404:
            return "NOT FOUND";
        default:
            return "UNKNOWN";
    }
}

int serialize_response(struct HTTPResponse *response, char *buffer) {
    int idx = sprintf(buffer, "HTTP/1.1 %d %s\n"
                              "Content-Type: %s\n"
                              "Content-Length: %ld\n",
                      response->status, get_status_code_string(response->status), response->content_type, strlen(response->content));

    idx += sprintf(buffer + idx, "X-Powered-By: C-Web-Server\n");

    idx += sprintf(buffer + idx, "\n%s", response->content);

    return idx;
}

void send_http_response(int sockfd, struct HTTPResponse *response) {
    char *buffer = malloc(BUFFER_SIZE);
    int num_bytes = serialize_response(response, buffer);
    write(sockfd, buffer, num_bytes);
    free(buffer);
}

void handle_connection(int sockfd, struct sockaddr_in *client_addr) {
    char *buffer = malloc(BUFFER_SIZE);

    ssize_t bytes_read = read(sockfd, buffer, BUFFER_SIZE);
    if (bytes_read == -1) {
        perror("Error reading from socket");
        return;
    } else {
        printf("Read %ld bytes from socket with %s: %s\n", bytes_read, inet_ntoa(client_addr->sin_addr), buffer);
    }

    struct HTTPResponse response;
    response.status = 404;
    response.content_type = "text/html";
    response.content = "<h1>404 Not Found</h1><p>The requested resource could not be found.</p>";

    send_http_response(sockfd, &response);
    close(sockfd);
}
