#pragma once

#include <sys/socket.h>

int s_socket(int domain, int type, int protocol);

int s_bind(int socket, const struct sockaddr* addr, socklen_t len);

ssize_t s_recvfrom(int sock, void* buf, size_t len, int flags,
                   struct sockaddr* addr, socklen_t* addrlen);

ssize_t s_sendto(int sock, const void* buf, size_t len, int flags,
                 const struct sockaddr* addr, socklen_t addrlen);
