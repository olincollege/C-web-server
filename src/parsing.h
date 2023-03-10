#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <stddef.h>

struct http_request {
    char *method;
    char *path;
    char *query_string;
    char *headers;
    char *body;
};

struct http_response {
    size_t header_size;
    size_t body_size;
    char *status;
    char *headers;
    char *body;
};

/**
 * Parses an HTTP request string into a http_request struct
 * 
 * @param request_str A string containing the HTTP request
 * @return A http_request struct containing the parsed request
 */
struct http_request *parse_request(char *request_str);

/**
* Frees the memory allocated to a http_request struct
 *
 * @param req The http_request struct to free
 */
void free_request(struct http_request *req);

/**
 * Prints the contents of a http_request struct to stdout
 *
 * @param req The http_request struct to print
 */
void print_request(struct http_request *req);

// response functions

/**
 * Creates a new http_response struct
 *
 * This function will allocate memory for the http_response struct, and
 * set the header_size and body_size fields to 0.
 *
 * When you are done with the struct, you must free it with free_response.
 *
 * @return A pointer to the new http_response struct
 */
struct http_response *new_response(void);

/**
 * Adds a header to a http_response struct
 *
 * If no headers have been added to the response, this function will allocate memory for the headers.
 * If headers have already been added to the response, this function will reallocate memory for the headers.
 *
 * This function also sets the header_size field of the http_response struct.
 *
 * Per the HTTP specification, headers may be repeated, so this function does not check if a header has already been added.
 *
 * Use write_content_length_to_response for the Content-Length header.
 * DO NOT USE THIS FUNCTION FOR THE Content-Length HEADER.
 *
 * @param res The http_response struct to add the header to
 * @param header_name The name of the header, like "Content-Type"
 * @param header_value The value of the header
 */
void add_header_to_response(struct http_response *res, char *header_name, char *header_value);

/**
 * Adds a static status to a http_response struct.
 * It takes a pointer to a string, so the string must be static.
 *
 * This function will allocate memory for the status so it can be freed later by free_response.
 *
 * @param res The http_response struct to add the status to
 * @param status The status to add, like "200 OK"
 */
void add_static_status_to_response(struct http_response *res, char *status);

/**
 * Adds a static body to a http_response struct.
 *
 * This function will allocate memory for the body so it can be freed later by free_response.
 *
 * @param res The http_response struct to add the body to
 * @param body The body to add
 */
void add_static_body_to_response(struct http_response *res, char *body);

/**
 * Writes the Content-Length header to a http_response struct.
 *
 * This function uses the body_size field of the http_response struct to determine the length of the body.
 * Ensure that this function is called after adding the body and its size to the http_response struct.
 *
 * Only call this function once per response. It is not idempotent.
 *
 * @param res The http_response struct to add the header to
 */
void write_content_length_to_response(struct http_response *res);

/**
 * Gets the size of the buffer needed to serialize a http_response struct.
 *
 * This function uses the header_size and body_size fields of the http_response struct to determine the size of the buffer.
 * It automatically includes all space needed for the HTTP version, status, newlines, headers, and body.
 *
 * @param res The http_response struct to get the size of
 * @return The size of the buffer needed to serialize the http_response struct
 */
size_t get_serialized_response_buffer_size(struct http_response *res);

/**
 * Serializes a http_response struct into a buffer.
 *
 * Make sure that this buffer is large enough to hold the serialized response-- use
 * get_serialized_response_buffer_size to get the size of the buffer needed.
 *
 * @param res The http_response struct to serialize
 * @param response_buf The buffer to serialize the http_response struct into
 * @return The total number of bytes written to the buffer (should nearly match its size)
 */
size_t serialize_response(struct http_response *res, char *response_buf);

/**
 * Frees the memory allocated to a http_response struct
 *
 * @param res The http_response struct to free
 */
void free_response(struct http_response *res);

#endif
