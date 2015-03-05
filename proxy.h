#ifndef __PROXY_H__
#define __PROXY_H__ 1

#include <stdbool.h>

struct proxy
{
  struct tcpConnection *source;
  struct tcpCOnnection *destination;
};

bool proxy_connect (int, int);

#endif /* __PROXY_H__ */
