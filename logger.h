#ifndef __LOGGER_H__
#define __LOGGER_H__ 1

enum log_level
{
  FATAL,
  ERROR,
  WARN,
  INFO
};

void logger (enum log_level level, const char *fmt, ...);

#endif /* __LOGGER_H__ */
