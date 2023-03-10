#include "connection.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "handler.h"
#include "parsing.h"

#define BUFFER_SIZE 4096

char *get_status_code_string(int status_code) {
    switch (status_code) {
        case 200:
            return "200 OK";
        case 204:
            return "204 Created";
        case 400:
            return "400 Bad Request";
        case 404:
            return "404 Not Found";
        case 405:
            return "405 Method Not Allowed";
        case 500:
            return "500 Internal Server Error";
        case 501:
            return "501 Not Implemented";
        default:
            return "500 Unknown";
    }
}

char *get_content_type_from_ext(char *ext) {
    if (strcmp(ext, "txt") == 0) {
        return "text/plain";
    } else if (strcmp(ext, "html") == 0) {
        return "text/html";
    } else if (strcmp(ext, "css") == 0) {
        return "text/css";
    } else if (strcmp(ext, "js") == 0) {
        return "text/javascript";
    } else if (strcmp(ext, "png") == 0) {
        return "image/png";
    } else if (strcmp(ext, "jpg") == 0) {
        return "image/jpeg";
    } else if (strcmp(ext, "gif") == 0) {
        return "image/gif";
    } else if (strcmp(ext, "svg") == 0) {
        return "image/svg+xml";
    } else if (strcmp(ext, "ico") == 0) {
        return "image/x-icon";
    } else if (strcmp(ext, "json") == 0) {
        return "application/json";
    } else if (strcmp(ext, "pdf") == 0) {
        return "application/pdf";
    } else if (strcmp(ext, "zip") == 0) {
        return "application/zip";
    } else if (strcmp(ext, "xml") == 0) {
        return "application/xml";
    } else if (strcmp(ext, "mp3") == 0) {
        return "audio/mpeg";
    } else if (strcmp(ext, "mp4") == 0) {
        return "video/mp4";
    } else if (strcmp(ext, "webm") == 0) {
        return "video/webm";
    } else if (strcmp(ext, "ogg") == 0) {
        return "video/ogg";
    } else {
        return "application/octet-stream";
    }
}

void internal_server_error(struct http_response *response) {
    add_static_status_to_response(response, get_status_code_string(500));
    add_static_body_to_response(response, "<h1>Internal Server Error</h1>"
                                          "<p>Something went wrong on our end. Check the server console.</p>");
    add_header_to_response(response, "Content-Type", "text/html");
}


void send_http_response(int sockfd, struct http_response *response) {
    char *response_buffer = malloc(get_serialized_response_buffer_size(response));
    size_t response_bytes = serialize_response(response, response_buffer);
    write(sockfd, response_buffer, response_bytes);
    //    printf("Sending %zu-byte response with body size %zu\n", response_bytes, response->body_size);
    free(response_buffer);
}


void handle_connection(int sockfd, struct sockaddr_in *client_addr) {
    char *buffer = malloc(BUFFER_SIZE);

    size_t bytes_read = read(sockfd, buffer, BUFFER_SIZE);
    if (bytes_read == -1) {
        perror("Error reading from socket");
        return;
    }

    // parse request
    struct http_request *request = parse_request(buffer);
    //    print_request(request);

    // create and allocate response
    struct http_response *response = new_response();

    // pass the handler the request and response
    // the handler will fill in the response, and we'll send it back
    int handler_error = handle_request(request, response);
    if (handler_error != 0) {
        // handler failed, send 500
        perror("Encountered error in handler");
        internal_server_error(response);
    }

    if (response->status == NULL) {
        // handler didn't set status, set 200
        if (response->body == NULL) {
            add_static_status_to_response(response, get_status_code_string(204));
        } else {
            add_static_status_to_response(response, get_status_code_string(200));
        }
    }

    write_content_length_to_response(response);
    add_header_to_response(response, "Connection", "close");
    add_header_to_response(response, "X-Powered-By", "Sanmoenta C Web Server");
    // send the response
    send_http_response(sockfd, response);

    // free the request and response
    free_request(request);
    free_response(response);

    // close the socket, we're not implementing keep-alive
    close(sockfd);
}
