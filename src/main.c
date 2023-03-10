#include "connection.h"
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 80

int sockfd = 0;

void handle_sigterm(__attribute__((unused)) int signal) {
    printf("Received SIGTERM, shutting down server");
    close(sockfd);
    exit(0);
}

struct sockaddr_in create_address(int port) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;               // AF_INET is the internet address family
    addr.sin_port = htons(port);             // htons converts to network byte order, which is big endian
    addr.sin_addr.s_addr = htonl(INADDR_ANY);// inet_addr converts a string to an in_addr struct
    return addr;
}

int main(void) {
    signal(SIGINT, handle_sigterm);
    signal(SIGTERM, handle_sigterm);
    signal(SIGSTOP, handle_sigterm);
    signal(SIGABRT, handle_sigterm);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        perror("Error creating socket");
        return 1;
    }

    // create address struct
    // this lets us specify the port and address to listen on
    struct sockaddr_in addr = create_address(PORT);

    int bind_result = bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));
    if (bind_result == -1) {
        perror("Error binding socket");
        return 1;
    } else {
        printf("Socket bound successfully to port %d\n", PORT);
    }

    int listen_result = listen(sockfd, 10);// 10 is the backlog, the maximum number of pending connections
    if (listen_result == -1) {
        perror("Error listening on socket");
        return 1;
    } else {
        printf("Socket listening on port %d\n", PORT);
    }

    while (1) {
        puts("Waiting for connection...");
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_sockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
        if (client_sockfd == -1) {
            perror("Error accepting connection");
            return 1;
        } else {
            printf("Accepted connection from %s\n", inet_ntoa(client_addr.sin_addr));
        }

        handle_connection(client_sockfd);
    }
}
