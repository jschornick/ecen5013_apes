/*
  File        : hw3.c
  Description : APES Homework 3

  Multithreaded...

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <stdio.h>
#include <stdarg.h>  // variable args
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "linked_list.h"

char default_logfile[] = "logfile";

pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct thread_info {
    pthread_t pthread_tid;
    pid_t linux_tid;
    uint8_t app_tid;
    const char *name;
    char *log_filename;  // const?
    FILE *p_logfile;
} thread_info_t;

typedef enum thread_name {
    THREAD_MAIN = 0,
    THREAD_1,
    THREAD_2,
    THREAD_MAX
} thread_name_t;

const char *thread_name[] = { "Main", "T1", "T2" };

thread_info_t thread_info[THREAD_MAX];

void logit( const char *format, ... );
void log_with_tinfo( thread_info_t *tinfo, const char*format, ...);
void log_varg( thread_info_t *tinfo, const char*format, va_list args);

void thread_init( thread_info_t *tinfo);
void *tf1( void *data );
void *tf2( void *data );

pthread_key_t tinfo_key;

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

int main( int argc, char *argv[] )
{

    thread_info_t *tinfo;
    tinfo = &thread_info[THREAD_MAIN];

    pthread_key_create(&tinfo_key, thread_cleanup);

    // Get log file name from the command line or use a default
    // We're populating the name in tinfo since the assignment requires that
    // this common parameter be passed into each thread
    if ( argc < 2 ) {
        tinfo->log_filename = default_logfile;
    } else {
        tinfo->log_filename = argv[1];
    }

    for(thread_name_t name = 0; name<THREAD_MAX; name++) {
        thread_info[name].app_tid = name;
        thread_info[name].log_filename = thread_info[THREAD_MAIN].log_filename;
    }

    //log_init?
    pthread_mutex_init(&log_mutex, NULL);  // NULL == default attrs

    // Even though this is the main thread, initiailze our info just like any other thread
    thread_init(tinfo);

    // *** main init done ***

    pthread_t t1, t2;

    // thread struct, thread attrs, func, arg
    if( pthread_create( &t1, NULL, tf1, &thread_info[THREAD_1] ) ) {
        logit( "Thread 1 create fail!\n" );
    }

    if( pthread_create( &t2, NULL, tf2, &thread_info[THREAD_2] ) ) {
        logit( "Thread 2 create fail!\n" );
    }

    // thread struct, void **retval
    pthread_join( t1, NULL );
    pthread_join( t2, NULL );

    // register with atexit?
    pthread_exit(0);

    return 0;
}

void *tf1( void *data )
{

    thread_info_t *tinfo = data;

    thread_init( tinfo );

    for( int i=0; i<2000; i++ ) {
        logit( "Thread 1 is the first thread\n" );
    }
    return NULL;
}

void *tf2( void *data )
{
    thread_info_t *tinfo = data;

    thread_init( tinfo );

    for( int i=0; i<2000; i++ ) {
        logit( "Thread 2 is a happy little thread\n" );
    }
    return NULL;
}


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
    // We can't be 100% certain when the file stream will be flushed, so append to the stream and flush in one atomic action
    // this guantees that complete log lines are added to the log file
        pthread_mutex_lock(&log_mutex);
    fprintf( tinfo->p_logfile, "[%s] ", tinfo->name);
    vfprintf( tinfo->p_logfile, format, args );
    fflush( tinfo->p_logfile );
    pthread_mutex_unlock(&log_mutex);
}
