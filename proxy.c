#define _GNU_SOURCE /* required for POLLRDHUP event */
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include "tcp.h"

#define BUFSIZE 2000

void
__pump(struct tcpConnection *source, struct tcpConnection *destination) {
  char buf[BUFSIZE];
  int len = tcp_read(source, buf, BUFSIZE);
  if (len > 0) {
    tcp_write(destination, buf, len);
  }

  printf("pumped %d bytes\n", len);
}

bool
proxy_connect(struct tcpConnection *source, struct tcpConnection *destination) {
  struct pollfd fds[2];

  fds[0].fd = source->fd;
  fds[0].events = POLLIN | POLLRDHUP;

  fds[1].fd = destination->fd;
  fds[1].events = POLLIN | POLLRDHUP;

  while (1) {
    int res = poll(fds, 2, -1);
    
    if (res == -1) {
      perror("poll() error: ");
    } else if (res) {
      if (fds[0].revents & POLLIN) {
	__pump(source, destination);
      }

      if (fds[1].revents & POLLIN) {
	__pump(destination, source);
      }

      if (fds[0].revents & POLLRDHUP ||
	  fds[1].revents & POLLRDHUP) {
      	return true;
      }
    }
  }

  return true;
}
