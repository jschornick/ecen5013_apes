/*
  File        : threads.c
  Description : APES Homework 3

  General threading support functions

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
  https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "logging.h"
#include "threads.h"

pthread_key_t tinfo_key;
thread_info_t thread_info[THREAD_MAX];
const char *thread_name[] = { "Main", "T1", "T2" };

// finishes tinfo population and opens log
void thread_init( thread_info_t *tinfo)
{
    tinfo->linux_tid = syscall(SYS_gettid);
    tinfo->pthread_tid = pthread_self();
    tinfo->name = thread_name[tinfo->app_tid];

    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t );

    pthread_setspecific(tinfo_key, tinfo );

    tinfo->p_logfile = fopen( tinfo->log_filename, "a" );
    logit( "Opened logfile, fd = %d\n", fileno(tinfo->p_logfile));
    logit( "App TID: %u, Linux TID: %d, pthread ID: %lu\n",
           tinfo->app_tid, tinfo->linux_tid, tinfo->pthread_tid );

    logit( "Start time: %u.%09u\n", t.tv_sec, t.tv_nsec );
}

void thread_cleanup(void *data)
{
    thread_info_t *tinfo = data;
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t );
    // our thread local storage is no longer valid, but the allocation is still valid
    log_with_tinfo(tinfo, "Exit time: %u.%09u\n", t.tv_sec, t.tv_nsec );
}
