/*
  File        : logging.h
  Description : APES Homework 3

  Logging functions

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
  https://github.com/jschornick/ecen5013_apes/hw3
*/


#ifndef _LOGGING_H
#define _LOGGING_H

#include "threads.h"

extern char default_logfile[];

void logit( const char *format, ... );
void log_with_tinfo( thread_info_t *tinfo, const char*format, ...);
void log_varg( thread_info_t *tinfo, const char*format, va_list args);

#endif /* _LOGGING_H */
