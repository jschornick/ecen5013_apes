/*
  File        : threads.c
  Description : APES Homework 3

  General threading support functions

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <pthread.h>
#include <unistd.h>   // for gettid
#include <sys/syscall.h>
#include <time.h>

#include "logging.h"
#include "threads.h"


pthread_key_t tinfo_key;
thread_info_t thread_info[THREAD_MAX];
const char *thread_name[] = { "Main", "Letters", "Metrics" };


thread_info_t * init_threading( char *log_filename )
{
    thread_info_t *tinfo;
    tinfo = &thread_info[THREAD_MAIN];

    // This TLS key allows each thread to have a pointer to its thread_info structure.
    pthread_key_create(&tinfo_key, thread_cleanup);

    tinfo->log_filename = log_filename;

    for(thread_name_t name = 0; name<THREAD_MAX; name++) {
        thread_info[name].app_tid = name;
        thread_info[name].log_filename = thread_info[THREAD_MAIN].log_filename;
    }

    // Even though this is the main thread, initiailze our info just like any other thread
    thread_init(tinfo);

    return tinfo;
}


void thread_init( thread_info_t *tinfo)
{
    tinfo->linux_tid = syscall(SYS_gettid);
    tinfo->pthread_tid = pthread_self();
    tinfo->name = thread_name[tinfo->app_tid];

    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t );

    pthread_setspecific(tinfo_key, tinfo );

    tinfo->p_logfile = fopen( tinfo->log_filename, "a" );
    logit( "Opened logfile (fd = %d), start time: %u.%09u\n",
           fileno(tinfo->p_logfile), t.tv_sec, t.tv_nsec );
    logit( "App TID: %u, Linux TID: %d, pthread ID: %lu\n",
           tinfo->app_tid, tinfo->linux_tid, tinfo->pthread_tid );

}


void thread_cleanup(void *data)
{
    thread_info_t *tinfo = data;
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t );

    // NOTE: Can't use logit() in cleanup since TLS keys are invalid!

    if (tinfo->timer) {
        log_with_tinfo( tinfo, "Deleting POSIX timer\n" );
        timer_delete(tinfo->timer);
    }

    // our thread local storage is no longer valid, but the allocation is still valid
    log_with_tinfo( tinfo, "Closing logfile (fd = %d), exit time: %ld.%09ld\n",
           fileno(tinfo->p_logfile), t.tv_sec, t.tv_nsec );
}
