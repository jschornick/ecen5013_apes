/*
  File        : logging.c
  Description : APES Homework 3

  Logging system

  The logging system is built around the requirement that all threads will
  individually open the log file, and thus each will have a file handle to
  a unique file stream.

  An atomic lock/write/flush/unlock pattern is used to guarantee that no
  interleaving occurs during concurrent calls to the logging API.

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <pthread.h>
#include <stdarg.h>  // variable args
#include <time.h>

#include "threads.h"
#include "logging.h"


char default_logfile[] = "logfile";

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;


void logit( const char *format, ... )
{
    va_list args;
    va_start(args, format); // populate va_list with optional args

    thread_info_t *tinfo = pthread_getspecific(tinfo_key);
    log_varg(tinfo, format, args );

    va_end(args);  // cleanup
}


void log_with_tinfo( thread_info_t *tinfo, const char*format, ...)
{

    va_list args;
    va_start(args, format); // populate va_list with optional args

    log_varg(tinfo, format, args );

    va_end(args);  // cleanup
}


void log_varg( thread_info_t *tinfo, const char*format, va_list args)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t );

    // We can't be 100% certain when the file stream will be flushed, so append to the stream and flush in one atomic action
    // this guantees that complete log lines are added to the log file
    pthread_mutex_lock(&log_mutex);

    fprintf( tinfo->p_logfile, "%07ld.%09ld ", t.tv_sec, t.tv_nsec );
    fprintf( tinfo->p_logfile, "[%s] ", tinfo->name);
    vfprintf( tinfo->p_logfile, format, args );
    fflush( tinfo->p_logfile );

    pthread_mutex_unlock(&log_mutex);
}
