#include "tcp.h"
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

struct tcpConnection* tcp_connect(char* host, char* port) {
    int sockfd;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(host, port, &hints, &res);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    connect(sockfd, res->ai_addr, res->ai_addrlen);

    struct tcpConnection* conn = (struct tcpConnection*) malloc(sizeof(struct tcpConnection));
    conn->fd = sockfd;
    return conn;
}

struct tcpConnection* tcp_listen(char* host, char* port) {
    int sockfd;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(host, port, &hints, &res);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    listen(sockfd, 10);

    struct tcpConnection* conn = (struct tcpConnection*) malloc(sizeof(struct tcpConnection));
    conn->fd = sockfd;
    return conn;
}

struct tcpConnection* tcp_accept(struct tcpConnection* conn) {
    struct tcpConnection* in = (struct tcpConnection*) malloc(sizeof(struct tcpConnection));
    in->fd = accept(conn->fd, NULL, 0);
    return in;
}

int tcp_read(struct tcpConnection* conn, char* buf, int len) {
    return read(conn->fd, buf, len);
}

int tcp_write(struct tcpConnection* conn, char* buf, int len) {
    write(conn->fd, buf, len);
}

void tcp_close(struct tcpConnection* conn) {
    close(conn->fd);
}
