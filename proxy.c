#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "tcp.h"

#define BUFSIZE 1000

void
__pump(struct tcpConnection *source, struct tcpConnection *destination) {
  char buf[BUFSIZE];
  int len = tcp_read(source, buf, BUFSIZE);
  if (len > 0) {
    tcp_write(destination, buf, len);
  } else {
  }
  printf("pumped %d bytes\n", len);
}

bool
proxy_connect(struct tcpConnection *source, struct tcpConnection *destination) {
  puts("configuring proxy...");
  while (1) {
    struct timeval timeout;
    fd_set fdset_reads;
    
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    FD_ZERO(&fdset_reads);
    FD_SET(source->fd, &fdset_reads);
    FD_SET(destination->fd, &fdset_reads);

    puts("selecting...");
    int res = select((source->fd + destination->fd) + 1, &fdset_reads, NULL, NULL, NULL);

    if (res == -1) {
      perror("select() error");
    } else if (res) {
      // check if the client is sending data
      if (FD_ISSET(source->fd, &fdset_reads)) {
	puts("source sending data");
	__pump(source, destination);
      }

      // check if the destination is sending a response
      if (FD_ISSET(destination->fd, &fdset_reads)) {
	puts("destination sending data");
	__pump(destination, source);
      }
    } else {
      puts("no data in3 a while...bye!");
      break;
    }
  }

  return true;
}
