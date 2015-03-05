#ifndef __LOGGER_H__
#define __LOGGER_H__ 1

enum log_level
{
  FATAL,
  ERROR,
  INFO
};

void log (enum log_level, const char *message);

#endif /* __LOGGER_H__ */
