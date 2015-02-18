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
  
  socks5_auth_print(auth_request);
  
  /* only support method 00 for now */
  if (auth_request->methods[0] == AUTH_NONE) {
    socks5_write_auth(client, AUTH_NONE);
    struct socks5_request *request = socks5_read_request(client);
    socks5_request_print(request);
  } else {
    socks5_write_auth(client, AUTH_NOT_ACCEPTABLE);
    perror("auth not supported");
  }
}

int 
main (int argc, char **argv) {
  init();
  return 0;
}
