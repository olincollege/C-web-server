#include "handler.h"
#include "connection.h"
#include "readfiles.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX_PATH_LEN = 256;

void file_not_found(struct http_response *response) {
    add_static_status_to_response(response, get_status_code_string(404));
    add_static_body_to_response(response, "<h1>File Not Found</h1>"
                                          "<p>The file you requested could not be found.</p>");
    add_header_to_response(response, "Content-Type", "text/html");
}

int handle_request(struct http_request *request, struct http_response *response) {
    size_t path_len = strlen(request->path);

    if(path_len > MAX_PATH_LEN) {
        char *body = malloc(75);
        add_static_status_to_response(response, get_status_code_string(404));
        response->body_size = sprintf(body, "<h1>400 Bad Request</h1><p>Path too long. Max %d characters.</p>", MAX_PATH_LEN);
        add_static_body_to_response(response, body);
        add_header_to_response(response, "Content-Type", "text/html");
        return 0;
    }

    // accounts for either ./index.html\0 or ./<path>\0
    char* path = malloc(path_len == 1 ? 13 : path_len + 4);
    size_t final_path_len = path_len == 1 ? 13 : path_len + 4;

    if(path_len == 1) {
        // path is /, correct to index.html
        strcpy(path, "./index.html");
    } else {
        // remove first /
        path[0] = '.';
        strcpy(path+1, request->path);
    }

    readfiles_error ferr = READFILES_OK;
    fwmd *filedata = create_file_with_metadata(path, &ferr);
    if (ferr != READFILES_OK) {
        switch (ferr) {
            case READFILES_FILE_NOT_FOUND: {
                char *body = malloc(final_path_len + 59);
                add_static_status_to_response(response, get_status_code_string(404));
                response->body_size = sprintf(body, "<h1>404 Not Found</h1><p>The file %s could not be found.</p>", path);
                add_static_body_to_response(response, body);
                add_header_to_response(response, "Content-Type", "text/html");
                return 0;
            }
            default: {
                printf("Error reading file: %d (%s)\n", ferr, readfiles_error_string(ferr));
                perror("Error reading file");
                return ferr;
            }
        }
    }


    if(filedata->is_dir) {
        char* body = malloc(final_path_len + 93);
        add_static_status_to_response(response, get_status_code_string(501));
        response->body_size = sprintf(body, "<h1>501 Not Implemented</h1><p>The file %s is a directory. Contents listing coming soon.</p>", path);
        add_static_body_to_response(response, body);
        add_header_to_response(response, "Content-Type", "text/html");
        return 0;
    }

    if(request->query_string != NULL && strcmp(request->query_string, "metadata") == 0) {
        char* body = malloc(128);
        response->body_size = sprintf(body, "<h1>File metadata: %s</h1><ul><li>Extension: %s</li><li>Size: %zub</li></ul>", path, filedata->file_ext, filedata->buf_size);
        add_static_body_to_response(response, body);
        add_header_to_response(response, "Content-Type", "text/html");
    } else {
        response->body_size = filedata->buf_size;
        response->body = filedata->content_buf;
        printf("Content type: %s from extension %s\n", get_content_type_from_ext(filedata->file_ext), filedata->file_ext);
        add_header_to_response(response, "Content-Type", get_content_type_from_ext(filedata->file_ext));
    }

    free_file_with_metadata(filedata, true);
    free(path);

    return 0;
}
