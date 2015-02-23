#ifndef __PROXY_H__
#define __PROXY_H__ 1

#include <stdbool.h>

struct proxy {
  struct tcpConnection *source;
  struct tcpCOnnection *destination;
};

bool
proxy_connect(struct tcpConnection *source, struct tcpConnection *destination);

#endif /* __PROXY_H__ */
