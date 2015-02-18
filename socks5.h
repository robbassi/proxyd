#ifndef __SOCKS5_H__
#define __SOCKS5_H__ 1

#define SOCKS5_AUTH_MIN_PACKET_SIZE 257

enum socks5_auth_method {
  AUTH_NONE           = 0x00,
  AUTH_GSSAPI         = 0x01,
  AUTH_USERPASS       = 0x02,
  AUTH_NOT_ACCEPTABLE = 0xFF
};

enum socks5_command {
  CMD_CONNECT   = 0x01,
  CMD_BIND      = 0x02,
  CMD_UDP_ASSOC = 0x03
};

enum socks5_addr_type {
  ATYP_IPV4   = 0x01,
  ATYP_DOMAIN = 0x03,
  ATYP_IPV6   = 0x04
};

enum socks5_response_code {
  RES_OK,
  RES_FAIL,
  RES_FORBIDDEN,
  RES_NET_UNREACHABLE,
  RES_HOST_UNREACHABLE,
  RES_REFUSED,
  RES_TTL_EXP,
  RES_BAD_CMD,
  RES_BAD_ATYP
};

struct socks5_auth {
  char version;
  char nmethods;
  char methods[];
};

struct socks5_request {
  char version;
  char command;
  char reserved;
  char address_type;
  union {
    char ipv4[4];
    char ipv6[16];
    char *domain;
  } bind_address;
};

struct socks5_auth *
socks5_read_auth (struct tcpConnection *client);

#endif /* __SOCKS5_H__ */
