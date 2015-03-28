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
  bool success = false;
  struct tcpConnection *destination = NULL;
  char portbuf[100];
  snprintf (portbuf, 10, "%d", request->bind_port.number);

  switch (request->address_type)
    {
    case ATYP_IPV4:
      break;
    case ATYP_IPV6:
      break;
    case ATYP_DOMAIN:
      logger (INFO, "connecting to %s:%s", request->bind_address.domain.name,
	      portbuf);
      destination = tcp_connect (request->bind_address.domain.name, portbuf);

      // could not connect to host
      if (destination == NULL)
	{
	  socks5_write_request (client, RES_HOST_UNREACHABLE);
	  goto exit;
	}
      break;
    }

  if (destination == NULL)
    {
      socks5_write_request (client, RES_FAIL);
      perror ("could not connect to destination host");
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
      if (auth_request->methods[0] == AUTH_NONE)
	{
	  socks5_write_auth (client, AUTH_NONE);
	  success = true;
	}
      else
	{
	  perror ("auth not supported");
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
	    case CMD_BIND:
	      logger (WARN, "bind not supported");
	      break;
	    case CMD_UDP_ASSOC:
	      logger (WARN, "udp assoc. not supported");
	      break;
	    }

	  free (request);
	}
    }
  else
    {
      // socks5_write_auth(client, AUTH_NOT_ACCEPTABLE);
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
