#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../src/parsing.h"

// NOLINTBEGIN(*-magic-numbers)


char *example_post_request_text = "POST /path?query=value HTTP/1.1\r\n"
                                  "Host: localhost\r\n"
                                  "User-Agent: curl/7.64.1\r\n"
                                  "Accept: */*\r\n"
                                  "postdata";
char *example_post_request = NULL;

char *example_get_request_text = "GET /path?query=value HTTP/1.1\r\n"
                                 "Host: localhost\r\n"
                                 "User-Agent: curl/7.64.1\r\n"
                                 "Accept: */*\r\n";
char *example_get_request = NULL;

void setup_requests() {
    example_post_request = malloc(strlen(example_post_request_text) + 1);
    strcpy(example_post_request, example_post_request_text);

    example_get_request = malloc(strlen(example_get_request_text) + 1);
    strcpy(example_get_request, example_get_request_text);
}

void cleanup_requests() {
    free(example_post_request);
    example_post_request = NULL;

    free(example_get_request);
    example_get_request = NULL;
}

// Test parsing a POST request
Test(parse_post_request, parse_request, .init = setup_requests, .fini = cleanup_requests) {
    struct http_request *req = parse_request(example_post_request);
    cr_assert(eq(str, req->method, "POST"));
    cr_assert(eq(str, req->path, "/path"));
    cr_assert(eq(str, req->query_string, "query=value"));
    cr_assert(eq(str, req->headers, "Host: localhost\r\n"
                                    "User-Agent: curl/7.64.1\r\nAccept: */*"));
    cr_assert(eq(str, req->body, "postdata"));
}

// Test parsing a GET request
Test(parse_get_request, parse_request, .init = setup_requests, .fini = cleanup_requests) {
    struct http_request *req = parse_request(example_post_request);
    cr_assert(eq(str, req->method, "POST"));
    cr_assert(eq(str, req->path, "/path"));
    cr_assert(eq(str, req->query_string, "query=value"));
    cr_assert(eq(str, req->headers, "Host: localhost\r\n"
                                    "User-Agent: curl/7.64.1\r\nAccept: */*"));
    cr_assert(eq(ptr, req->body, NULL));
}

// NOLINTEND
