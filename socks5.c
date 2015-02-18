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
  char buf[2] = {5};
  buf[1] = method;
  return tcp_write(client, buf, 2);
}

struct socks5_request *
socks5_read_request(struct tcpConnection *client) {
  char *buf = (char *) malloc(265);
  int length = tcp_read(client, buf, 265);
  struct socks5_request *request = (struct socks5_request *) buf;

  if (request->address_type == ATYP_DOMAIN) {
    int name_size = request->bind_address.domain.length;
    request->bind_port.bytes[0] = request->bind_address.domain.name[name_size];
    request->bind_port.bytes[1] = request->bind_address.domain.name[name_size + 1];
    request->bind_port.number = ntohs(request->bind_port.number);
    request->bind_address.domain.name[name_size] = '\0';
  }

  return request;
};

/* debug functions */

void
socks5_auth_print (struct socks5_auth *auth_request) {
  printf("ver: %d\n", auth_request->version);
  printf("auth methods: \n");
  int i;
  for (i = 0; i < auth_request->nmethods; i++) 
    printf(" - %s\n", socks5_auth_method_desc[auth_request->methods[i]]);
}

void
socks5_request_print (struct socks5_request *request) {
  printf("version: %d\n", request->version);
  printf("command: %s\n", socks5_command_desc[request->command]);
  printf("address_type: %s\n", socks5_addr_type_desc[request->address_type]);
  printf("bind_address: ");
  switch (request->address_type) {
  case ATYP_IPV4:
    printf("%d.%d.%d.%d\n", 
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
    printf("%s\n", request->bind_address.domain.name);
    break;
  default:
    perror("unknown address type");
  }
  printf("bind_port: %d\n", request->bind_port.number);
}
