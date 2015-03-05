#ifndef LIBTCP_H
#define LIBTCP_H 1

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct tcpConnection
{
  int fd;
  int mutex;

};

struct tcpConnection *tcp_connect (char *host, char *port);
struct tcpConnection *tcp_listen (char *host, char *port);
struct tcpConnection *tcp_accept (struct tcpConnection *);
int tcp_read (struct tcpConnection *, char *buf, int len);
int tcp_write (struct tcpConnection *, char *buf, int len);
void tcp_close (struct tcpConnection *);

#endif /*LIBTCP_H */
