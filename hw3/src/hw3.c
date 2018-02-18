/*
  File        : hw3.c
  Description : APES Homework 3

  Multithreaded...

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <pthread.h>

#include "linked_list.h"
#include "threads.h"
#include "logging.h"

void *tf1( void *data );
void *tf2( void *data );

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


