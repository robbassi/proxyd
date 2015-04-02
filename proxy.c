#define _GNU_SOURCE		/* required for POLLRDHUP event */

#ifdef __APPLE__ 
#define _DARWIN_C_SOURCE 
#endif

#ifndef POLLRDHUP
#define POLLRDHUP 0x2000
#endif

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>

#include "tcp.h"
#include "logger.h"

#define BUFSIZE 20000
#define DELAYMS 100

int fwd (int sourcefd, int destfd)
{				
  char buf[BUFSIZE];
  int len = read (sourcefd, buf, BUFSIZE);
  if (len > 0)
    {
      int written = 0;
      while (written < len) 
	{
	  written += write (destfd, buf + written, len - written);
	}

      return len;
    }

  return 0;
}

bool proxy_connect (int sourcefd, int destfd)
{				
  struct pollfd fds[2];
  bool srcfin = false, destfin = false;

  fds[0].fd = sourcefd;
  fds[0].events = POLLIN;

  fds[1].fd = destfd;
  fds[1].events = POLLIN;

  while (1)
    {
      fds[0].revents = 0;
      fds[1].revents = 0;

      int res = poll (fds, 2, -1);

      if (res == -1)
	{
	  logger (ERROR, "poll() error: %s", strerror(errno));
	}
      else if (res)
	{

	  if (fds[0].revents & POLLIN)
	    {
	      
	      int len = fwd (fds[0].fd, fds[1].fd);

	      if (len == 0)
		{
		  return true;
		}
	    }

	  if (fds[1].revents & POLLIN)
	    {
	      int len = fwd (fds[1].fd, fds[0].fd);

	      if (len == 0)
		{
		  return true;
		}
	    }
	}
      usleep(DELAYMS);
    }

  return true;
}
