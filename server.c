#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "tcp.h"
#include "server.h"
#include "socks5.h"
#include "proxy.h"

bool
handle_connect(struct tcpConnection *client, struct socks5_request *request) {
  bool success = false;
  struct tcpConnection *destination = NULL;
  char portbuf[100];
  snprintf(portbuf, 10, "%d", request->bind_port.number);

  switch (request->address_type) {
  case ATYP_IPV4:
    break;
  case ATYP_IPV6:
    break;
  case ATYP_DOMAIN:
    printf("connecting to %s:%s\n", request->bind_address.domain.name, portbuf);
    destination = tcp_connect(request->bind_address.domain.name, portbuf);
    
    // could not connect to host
    if (destination == NULL) {
      socks5_write_request(client, RES_HOST_UNREACHABLE);
      goto exit;
    }
    break;
  }
  
  if (destination == NULL) {
    socks5_write_request(client, RES_FAIL);
    perror("could not connect to destination host");
    goto exit;
  }

  socks5_write_request(client, RES_OK);
  success = proxy_connect(client->fd, destination->fd);
  
 free_dest:
  free(destination);
  tcp_close(destination);
 exit:;
  return success;
}

bool
handle_auth(struct tcpConnection *client) {
  bool success = false;
  struct socks5_auth *auth_request = socks5_read_auth(client);

  if (auth_request != NULL) {
    socks5_auth_print(auth_request);
  
    /* only support method 00 for now */
    if (auth_request->methods[0] == AUTH_NONE) {
      socks5_write_auth(client, AUTH_NONE);
      success = true;
    } else {
      perror("auth not supported");
    }

    free(auth_request);
  }
  return success;
}

void *
handle_request(void *data) {
  struct tcpConnection *client = (struct tcpConnection *) data;
  bool auth_success = handle_auth(client);
  
  if (auth_success) {
    struct socks5_request *request = socks5_read_request(client);
    socks5_request_print(request);
    
    bool status;
    switch (request->command) {
    case CMD_CONNECT:
      status = handle_connect(client, request);
      if (!status) {
	    perror("connect failed");
      }
      break;
    case CMD_BIND:
      perror("bind not supported");
      break;
    case CMD_UDP_ASSOC:
      perror("udp assoc. not supported");
      break;
    }

    free(request);
  } else {
    // socks5_write_auth(client, AUTH_NOT_ACCEPTABLE);
    perror("auth failed");
  }

  free(data);
  return NULL;
}

void
start () {
  struct tcpConnection *sock = tcp_listen("localhost", "1080");
  while (1) {
    struct tcpConnection *client = tcp_accept(sock);
    pthread_t client_thread;
    pthread_create(&client_thread, NULL, handle_request, (void *)client);
  }
}

int 
main (int argc, char **argv) {
  start();
  return 0;
}
