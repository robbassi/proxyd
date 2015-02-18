#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcp.h"
#include "server.h"
#include "socks5.h"

void
init () {
  struct tcpConnection *sock = tcp_listen("localhost", "1080");
  struct tcpConnection *client = tcp_accept(sock);

  struct socks5_auth *auth_request = socks5_read_auth(client);

  printf("ver: %d\n", auth_request->version);
  printf("auth methods: \n");
  int i;
  for (i = 0; i < auth_request->nmethods; i++) 
    printf(" - %s\n", socks5_auth_method_desc[auth_request->methods[i]]);

  /* only support method 00 for now */
  if (auth_request->methods[0] == AUTH_NONE) {
    socks5_write_auth(client, AUTH_NONE);
    struct socks5_request *request = socks5_read_request(client);
    
    printf("version: %d\n", request->version);
    printf("command: %d\n", request->command);
    printf("address_type: %d\n", request->address_type);
    printf("bind_address: ");
    switch (request->address_type) {
    case ATYP_IPV4:
      printf("%d.d.%d.%d\n", 
	     request->bind_address.ipv4[0],
	     request->bind_address.ipv4[1],
	     request->bind_address.ipv4[2],
	     request->bind_address.ipv4[3]);
      break;
    case ATYP_IPV6:
      {
	int i;
	for (i = 0; i < 15; i++)
	  printf("%d:", request->bind_address.ipv6[i]);
      }
      printf("%d\n", request->bind_address.ipv6[15]);
      break;
    case ATYP_DOMAIN:
      {
	int len = request->bind_address.domain[0];
	int i;
	for (i = 1; i <= len; i++)
	  printf("%c", request->bind_address.domain[i]);
      }      
      printf("\n");
      break;
    default:
      perror("unknown address type");
    }
  } else {
    socks5_write_auth(client, AUTH_NOT_ACCEPTABLE);
    perror("auth not supported");
  }
  
  /*
    struct socks5_session *session = authenticate_client(client);
    struct socks5_request *request = socks5_read_request(session);
    
    switch (request->command) {
    case CONNECT:
    break;
    case BIND:
    break;
    case UDP_ASSOC:
    break;
    default:
    // unknown command
    }
  */
}

int 
main (int argc, char **argv) {
  init();
  return 0;
}
