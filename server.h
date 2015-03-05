#ifndef __SERVER_H__
#define __SERVER_H__ 1

#include <stdbool.h>

struct socks5_session
{
  bool authenticated;
  bool active;
  struct tcpConnection *client;
  struct tcpConnection *server;
};

extern char *socks5_auth_method_desc[];
extern char *socks5_command_desc[];
extern char *socks5_addr_type_desc[];
extern char *socks5_response_code_desc[];

#endif /* __SERVER_H__ */
