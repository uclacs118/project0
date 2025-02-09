#include "libsocket.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: client <hostname> <port> \n");
        exit(1);
    }

    /* Create sockets */
    int sockfd = s_socket(AF_INET, SOCK_DGRAM, 0);
    // use IPv4  use UDP
    // Error if socket could not be created
    if (sockfd < 0)
        return errno;

    // Set socket for nonblocking
    int flags = fcntl(sockfd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, flags);

    // Setup stdin for nonblocking
    flags = fcntl(STDIN_FILENO, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flags);

    /* Construct server address */
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // use IPv4
    // Only supports localhost as a hostname, but that's all we'll test on
    const char* addr = strcmp(argv[1], "localhost") == 0 ? "127.0.0.1" : argv[1];
    server_addr.sin_addr.s_addr = inet_addr(addr);
    // Set sending port
    int PORT = atoi(argv[2]);
    server_addr.sin_port = htons(PORT); // Big endian
    socklen_t s = sizeof(struct sockaddr_in);
    char buffer[1024];

    // Listen loop
    while (1) {
        // Receive from socket
        int bytes_recvd = s_recvfrom(sockfd, &buffer, sizeof(buffer), 0,
                                     (struct sockaddr*) &server_addr, &s);

        // Data available to write
        if (bytes_recvd > 0) {
            fprintf(stderr, "RECV %d\n", bytes_recvd);
            write(STDOUT_FILENO, buffer, bytes_recvd);
        }

        // Read from stdin
        int bytes_read = read(STDIN_FILENO, &buffer, sizeof(buffer));

        // Data available to send from stdin
        if (bytes_read > 0) {
            fprintf(stderr, "SEND %d\n", bytes_read);
            s_sendto(sockfd, &buffer, bytes_read, 0,
                   (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in));
        }
    }

    return 0;
}
