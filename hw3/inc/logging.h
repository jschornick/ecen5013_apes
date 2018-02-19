/*
  File        : logging.h
  Description : APES Homework 3

  Logging system function declarations

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#ifndef _LOGGING_H
#define _LOGGING_H

#include "threads.h"

extern char default_logfile[];

// Function: logit
//
// This is the primary logging interface for running threads. It requires that
// the thread have a valid TLS with a key `tinfo_key` associated with the fully
// initialized thread_info_t structure for the thread.
//
// The parameters behave identally to that of printf().
//
// param[in] format - printf-style format string
// param[in] ... - optional argument list for format string
void logit( const char *format, ... );

// Function: log_with_tinfo
//
// This is a secondary interface to the logger when the TLS has been
// invalidated, such as during thread termination.
//
// It behaves similar to logit(), but requires that a valid thread_info_t
// be passed in directly.
//
// param[in] tinfo  - pointer to a populated thread info struct
// param[in] format - printf-style format string
// param[in] ...    - optional argument list for format string
void log_with_tinfo( thread_info_t *tinfo, const char*format, ...);


// Function: log_varg
//
// This is the workhorse of the logging system. It is common to all interface
// functions and should not be called directly.
//
// Log lines passed in are written and flushed to the underlying log file as an
// atomic unit, so this should produce consistent log output even in a
// multithreaded environment.
//
// param[in] tinfo  - pointer to a populated thread info struct
// param[in] format - printf-style format string
// param[in] args   - va_list-style argument list for format string
void log_varg( thread_info_t *tinfo, const char*format, va_list args);

#endif /* _LOGGING_H */
