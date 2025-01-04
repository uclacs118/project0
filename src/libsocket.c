#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define UNUSED(x) (void) (x)

// Helper functions

struct sockaddr_un port_to_address(int port) {
    struct sockaddr_un un_addr = {};
    un_addr.sun_family = AF_UNIX;
    snprintf(un_addr.sun_path, sizeof(un_addr.sun_path), "/tmp/%d", port);

    return un_addr;
}

int address_to_port(const struct sockaddr_un* addr) {
    int port = 0;
    int num = sscanf(addr->sun_path, "/tmp/%d", &port);

    return num != 1 ? -1 : port;
}

ssize_t c_bind(int sock) {
    uint32_t r;
    int rfd = open("/dev/urandom", 'r');
    read(rfd, &r, sizeof(uint32_t));
    close(rfd);
    srand(r);
    int port = (rand() % 1000) + 9000;

    struct sockaddr_un un_addr = port_to_address(port);
    unlink(un_addr.sun_path);
    return bind(sock, (struct sockaddr*) &un_addr, sizeof(un_addr));
}

// Interface

int s_socket(int domain, int type, int protocol) {
    UNUSED(protocol);

    if (domain != AF_INET) {
        errno = EAFNOSUPPORT;
        return -1;
    }

    if (type != SOCK_DGRAM) {
        errno = EPROTOTYPE;
        return -1;
    }

    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    int sz = 200000;
    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &sz, sizeof(sz));
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &sz, sizeof(sz));

    return sock;
}

int s_bind(int sock, const struct sockaddr* addr, socklen_t len) {
    UNUSED(len);

    if (addr == NULL || addr->sa_family != AF_INET) {
        errno = EINVAL;
        return -1;
    }

    int port = ntohs(((struct sockaddr_in*) addr)->sin_port);
    struct sockaddr_un* un_addr =
        (struct sockaddr_un*) malloc(sizeof(struct sockaddr_un));
    *un_addr = port_to_address(port);
    unlink(un_addr->sun_path);
    return bind(sock, (struct sockaddr*) un_addr, sizeof(struct sockaddr_un));
}

ssize_t s_recvfrom(int sock, void* buf, size_t len, int flags,
                   struct sockaddr* addr, socklen_t* addrlen) {
    UNUSED(addrlen);

    if (addr == NULL) {
        errno = EINVAL;
        return -1;
    }

    struct sockaddr_un un_addr = {};
    socklen_t s = sizeof(un_addr);
    ssize_t ret =
        recvfrom(sock, buf, len, flags, (struct sockaddr*) &un_addr, &s);

    if (ret > 0) {
        int port = address_to_port(&un_addr);

        struct sockaddr_in* in_addr = (struct sockaddr_in*) addr;
        in_addr->sin_family = AF_INET;
        in_addr->sin_port = htons(port);
        in_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
    }

    return ret;
}

ssize_t s_sendto(int sock, const void* buf, size_t len, int flags,
                 const struct sockaddr* addr, socklen_t addrlen) {
    UNUSED(addrlen);

    c_bind(sock);

    if (addr == NULL || addr->sa_family != AF_INET) {
        errno = EINVAL;
        return -1;
    }
    struct sockaddr_un un_addr =
        port_to_address(ntohs(((struct sockaddr_in*) addr)->sin_port));

    errno = 0;
    int ret = sendto(sock, buf, len, flags, (struct sockaddr*) &un_addr,
                     sizeof(un_addr));

    return ret;
}
