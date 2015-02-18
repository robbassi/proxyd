#include "tcp.h"
#include "socks5.h"
#include <stdio.h>
#include <netinet/tcp.h>

char *socks5_auth_method_desc[256] = {
  [AUTH_NONE]           = "NO AUTHENTICATION REQUIRED",
  [AUTH_GSSAPI]         = "GSSAPI",
  [AUTH_USERPASS]       = "USERNBAME/PASSWORD",
  [AUTH_NOT_ACCEPTABLE] = "NO ACCEPTABLE METHODS"
};

char *socks5_command_desc[4] = {
  [CMD_CONNECT]   = "CONNECT",
  [CMD_BIND]      = "BIND",
  [CMD_UDP_ASSOC] = "UDP ASSOCIATE"
};

char *socks5_addr_type_desc[5] = {
  [ATYP_IPV4]   = "IP v4 Address",
  [ATYP_DOMAIN] = "DOMAINNAME",
  [ATYP_IPV6]   = "IP v6 Address"
};

char *socks5_response_code_desc[9] = {
  [RES_OK]               = "succeeded",
  [RES_FAIL]             = "general socks failure",
  [RES_FORBIDDEN]        = "connection not allowed by ruleset",
  [RES_NET_UNREACHABLE]  = "network unreachable",
  [RES_HOST_UNREACHABLE] = "host unreachable",
  [RES_REFUSED]          = "connection refused",
  [RES_TTL_EXP]          = "TTL expired",
  [RES_BAD_CMD]          = "command not supported",
  [RES_BAD_ATYP]         = "address type not supported"
};

struct socks5_auth *
socks5_read_auth (struct tcpConnection *client) {
  char *buf = (char *) malloc(SOCKS5_AUTH_MIN_PACKET_SIZE);
  int length = tcp_read(client, buf, SOCKS5_AUTH_MIN_PACKET_SIZE);
  struct socks5_auth *auth_pack = (struct socks5_auth *) buf;
  return auth_pack;
}

int
socks5_write_auth (struct tcpConnection *client, char method) {
  //int one = 1;
  //setsockopt(client->fd, SOL_TCP, TCP_NODELAY, &one, sizeof(one));

  char buf[2] = {5};
  buf[1] = method;
  return tcp_write(client, buf, 2);
}

struct socks5_request *
socks5_read_request(struct tcpConnection *client) {
  char *buf = (char *) malloc(255);
  int length;
  do {
    length = tcp_read(client, buf, 255);
  } while (length == 0);

  if (length > 0) {
    int i;
    for (i = 0; i < 255; i++)
      printf("%c", buf[i]);
    printf("\n");
  } else {
    perror("no response");
  }

  /* TODO: realloc */
  struct socks5_request *request = (struct socks5_request *) buf;
  
  if (request->address_type == ATYP_DOMAIN) {
    request->bind_address.domain = &(request->bind_address);
  }

  return request;
};


/*
int 
main (int argc, char **argv) {
  struct auth {
    char ver;
    char n;
    char methods[];
  };
  char buf[] = {05, 03, 97, 13, 42};
  struct auth *a = (struct auth *) buf;

  printf("ver: %d\n", a->ver);
  printf("auth methods: \n");
  int i;
  for (i = 0; i < a->n; i++) 
    printf(" - %d\n", a->methods[i]);

  return 0;
}
*/
