#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#include "logger.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define COLOR(c,m) c, m, KNRM
#define LVL_COLOR(l) ((l == INFO) ? KCYN : \
		      ((l == WARN) ? KYEL : \
		       ((l == ERROR) ? KMAG : KRED)))

const char *level_str[4] = {
    [FATAL] = "FATAL",
    [ERROR] = "ERROR",
    [WARN] = "WARN ",
    [INFO] = "INFO "
};

void _log (enum log_level, const char *, const char *);

void logger (enum log_level level, const char *fmt, ...)
{
  char buf_time[10];
  time_t currtime;
  struct tm *loctime;
  
  currtime = time (NULL);
  loctime = localtime (&currtime);

  strftime (buf_time, 10, "%H:%M:%S", loctime);

  char buf_msg[2048];
  va_list arg;

  va_start (arg, fmt);
  vsprintf (buf_msg, fmt, arg);
  va_end (arg);

  _log (level, buf_time, buf_msg);
}

void _log (enum log_level level, const char *time, const char *message) 
{
  printf("[%s%s%s|%s%d%s|%s%s%s] %s%s%s\n", 
	 COLOR(LVL_COLOR(level), level_str[level]),
	 COLOR(KGRN, getpid()),
	 COLOR(KBLU, time),
	 COLOR(KWHT, message));
}

/* int main (void) */
/* { */
/*   logger (INFO, "%s there %d", "hey", 42); */
/*   logger (ERROR, "oh shit!"); */
/*   logger (FATAL, "wtf !?!?!"); */
/*   return 0; */
/* } */
