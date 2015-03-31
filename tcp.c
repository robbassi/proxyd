#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdbool.h>

#include "tcp.h"

struct tcpConnection *tcp_connect (char *host, char *port)
{
  int sockfd;
  struct tcpConnection *conn = NULL;
  struct addrinfo hints, *res, *rp;

  memset (&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  int lookup_res = getaddrinfo (host, port, &hints, &res);

  if (lookup_res == 0)
    {
      
      for (rp = res; rp != NULL; rp = rp->ai_next) 
	{	
	  sockfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
	  if (sockfd == -1)
	    continue;

	  if (connect (sockfd, rp->ai_addr, rp->ai_addrlen) == 0)
	    break;
	}

      if (rp == NULL)
	{
	  return NULL;
	}

      conn = (struct tcpConnection *) malloc (sizeof (struct tcpConnection));
      conn->fd = sockfd;

      freeaddrinfo (res);
    }

  return conn;
}

struct tcpConnection *tcp_listen (char *host, char *port)
{
  int sockfd, reuseaddr = true;
  struct tcpConnection *conn = NULL;

  /* bind to INADDR_ANY */
  if (host == NULL)
    {
      struct sockaddr_in name;

      sockfd = socket (PF_INET, SOCK_STREAM, 0);

      name.sin_family = AF_INET;
      name.sin_port = htons (atoi(port));
      name.sin_addr.s_addr = htonl (INADDR_ANY);

      setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof reuseaddr);

      if (bind (sockfd, (struct sockaddr *) &name, sizeof (name)) == -1)
	{
	  goto fail1;
	}
    }
  /* bind to specific host */
  else 
    {
      struct addrinfo hints, *res, *rp;

      memset (&hints, 0, sizeof hints);
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;

      int lookup_res = getaddrinfo (host, port, &hints, &res);

      if (lookup_res == 0)
	{
	  for (rp = res; rp != NULL; rp = rp->ai_next) 
	    {
	      sockfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
	      if (sockfd == -1)
		continue;

	      if (bind (sockfd, rp->ai_addr, rp->ai_addrlen) == 0)
		break;
	    }

	  if (rp == NULL)
	    {
	      goto fail2;
	    }

	  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof reuseaddr);
	  freeaddrinfo (res);
	}
      else 
	{
	  goto fail2;
	}
    }

  if (listen (sockfd, 10) == -1)
    {
      goto fail1;
    }

  conn = (struct tcpConnection *) malloc (sizeof (struct tcpConnection));
  conn->fd = sockfd;

  return conn;
 fail1:
  close(sockfd);
 fail2:
  return NULL;
}

struct tcpConnection *tcp_accept (struct tcpConnection *conn)
{
  struct tcpConnection *in =
    (struct tcpConnection *) malloc (sizeof (struct tcpConnection));
  in->fd = accept (conn->fd, NULL, 0);
  return in;
}

int tcp_read (struct tcpConnection *conn, char *buf, int len)
{
  return read (conn->fd, buf, len);
}

int tcp_write (struct tcpConnection *conn, char *buf, int len)
{
  return write (conn->fd, buf, len);
}

void tcp_close (struct tcpConnection *conn)
{
  close (conn->fd);
}
