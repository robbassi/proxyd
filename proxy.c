#define _GNU_SOURCE /* required for POLLRDHUP event */
#include <stdio.h>
#include <stdbool.h>
#include <poll.h>
#include "tcp.h"

#define BUFSIZE 2000

void
__pump(int sourcefd, int destfd) { //, struct tcpConnection *destination) {
  char buf[BUFSIZE];
  int len = read(sourcefd, buf, BUFSIZE);
  if (len > 0) {
    write(destfd, buf, len);
  }

  printf("pumped %d bytes\n", len);
}

bool
proxy_connect(int sourcefd, int destfd) { //struct tcpConnection *source, struct tcpConnection *destination) {
  struct pollfd fds[2];

  fds[0].fd = sourcefd;
  fds[0].events = POLLIN | POLLRDHUP;

  fds[1].fd = destfd;
  fds[1].events = POLLIN | POLLRDHUP;

  while (1) {
    fds[0].revents = 0;
    fds[1].revents = 0;

    int res = poll(fds, 2, -1);
    
    if (res == -1) {
      perror("poll() error: ");
    } else if (res) {
      if (fds[0].revents & POLLRDHUP ||
	  fds[1].revents & POLLRDHUP) {
      	return true;
      }

      if (fds[0].revents & POLLIN) {
	    __pump(sourcefd, destfd);
      }

      if (fds[1].revents & POLLIN) {
	    __pump(destfd, sourcefd);
      }
    }
  }

  return true;
}
