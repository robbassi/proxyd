#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "tcp.h"
#include "server.h"
#include "socks5.h"
#include "proxy.h"
#include "logger.h"

bool handle_connect (struct tcpConnection *client,
		     struct socks5_request *request)
{
  int port;
  socklen_t len;
  bool success = false;

  struct sockaddr_storage addr;
  struct tcpConnection *destination = NULL;

  char portbuf[100], ipstr[INET6_ADDRSTRLEN];
  
  snprintf (portbuf, 10, "%d", request->bind_port.number);
  
  len = sizeof addr;
  getpeername (client->fd, (struct sockaddr*) &addr, &len);

  if (addr.ss_family == AF_INET) 
    {
      struct sockaddr_in *s = (struct sockaddr_in *) &addr;
      port = ntohs (s->sin_port);
      inet_ntop (AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
    } 
  else 
    { // AF_INET6
      struct sockaddr_in6 *s = (struct sockaddr_in6 *) &addr;
      port = ntohs (s->sin6_port);
      inet_ntop (AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
    }

  if (request->address_type == ATYP_DOMAIN)
    {
      logger (INFO, "%s:%d <=> %s:%s", ipstr, port, request->bind_address.domain.name, portbuf);
      destination = tcp_connect (request->bind_address.domain.name, portbuf);

      // could not connect to host
      if (destination == NULL)
	{
	  socks5_write_request (client, RES_HOST_UNREACHABLE);
	  goto exit;
	}
    }

  if (destination == NULL)
    {
      socks5_write_request (client, RES_FAIL);
      logger (WARN, "could not connect to destination host");
      goto exit;
    }

  socks5_write_request (client, RES_OK);
  success = proxy_connect (client->fd, destination->fd);

free_dest:
  tcp_close (destination);
  free (destination);
exit:;
  return success;
}

bool handle_auth (struct tcpConnection * client)
{
  bool success = false;
  struct socks5_auth *auth_request = socks5_read_auth (client);

  if (auth_request != NULL)
    {
      /* only support method 00 for now */
      int i;
      for (i = 0; !success && i < auth_request->nmethods; i++)
	{
	  if (auth_request->methods[i] == AUTH_NONE)
	    {
	      socks5_write_auth (client, AUTH_NONE);
	      success = true;
	    }	  
	}
      if (!success)
	{
	  logger (WARN, "auth not supported");
	}

      free (auth_request);
    }
  return success;
}

void *handle_request (void *data)
{
  struct tcpConnection *client = (struct tcpConnection *) data;
  bool auth_success = handle_auth (client);

  if (auth_success)
    {
      struct socks5_request *request = socks5_read_request (client);

      if (request != NULL)
	{
	  bool status;
	  switch (request->command)
	    {
	    case CMD_CONNECT:
	      status = handle_connect (client, request);
	      if (!status)
		{
		  logger (WARN, "connect failed");
		}
	      break;
	    default:
	      logger (WARN, "command not supported");
	      break;
	    }

	  free (request);
	}
    }
  else
    {
      socks5_write_auth (client, AUTH_NOT_ACCEPTABLE);
      logger (WARN, "auth failed");
    }

  tcp_close(client);
  free (data);
  return NULL;
}

void start ()
{
  struct tcpConnection *sock = tcp_listen (NULL, "1080");

  if (sock == NULL)
    {
      logger (WARN, "failed to bind");
      exit (1);
    }

  while (1)
    {
      struct tcpConnection *client = tcp_accept (sock);

      if (fork() == 0)
        {
          tcp_close (sock);
          handle_request ((void *) client);
          exit (0);
        }

      tcp_close (client);
    }
}

int main (int argc, char **argv)
{
  signal (SIGCHLD, SIG_IGN);
  start ();
  return 0;
}
