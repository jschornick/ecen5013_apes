/*
  File        : hw3.c
  Description : APES Homework 3

  Multithreaded...

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <pthread.h>
#include <signal.h>  // sigevent for posix timer
#include <time.h>
#include <semaphore.h>

#include "linked_list.h"
#include "threads.h"
#include "logging.h"

#include <unistd.h>   // sleep

#define METRICS_INTERVAL_MS 100

void *letter_counter( void *data );
void *metrics_reporter( void *data );

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

    pthread_t letters_thread, metrics_thread;

    // thread struct, thread attrs, func, arg
    if( pthread_create( &letters_thread, NULL, letter_counter, &thread_info[THREAD_LETTERS] ) ) {
        logit( "Failed to create letters thread!\n" );
    }

    if( pthread_create( &metrics_thread, NULL, metrics_reporter, &thread_info[THREAD_METRICS] ) ) {
        logit( "Failed to create metrics thread!\n" );
    }

    // thread struct, void **retval
    pthread_join( letters_thread, NULL );
    pthread_join( metrics_thread, NULL );

    // register with atexit?
    pthread_exit(0);

    return 0;
}


void *letter_counter( void *data )
{
    thread_info_t *tinfo = data;
    thread_init( tinfo );

    for( int i=0; i<100; i++ ) {
        logit( "Thread 1 is the first thread\n" );
    }
    return NULL;
}

uint32_t counter = 0;
sem_t metrics_sem;

// runs in its own thread, so do minimal work to return control to metrics reporter
void timer_func( union sigval val )
{
    // decrement mutex, cond variable here?
    counter++;
    sem_post(&metrics_sem);
}

void *metrics_reporter( void *data )
{
    thread_info_t *tinfo = data;
    thread_init( tinfo );

    struct sigevent event;
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = timer_func;
    event.sigev_value.sival_ptr = NULL;  // data value passed in when using SIGEV_THREAD?
    event.sigev_notify_attributes = NULL;

    timer_t timer;
    if (timer_create(CLOCK_MONOTONIC, &event, &timer) ) {
        logit( "Timer create failed\n" );
    }

    struct itimerspec ts;
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = METRICS_INTERVAL_MS * 1000000;  // expiration time in nanoseconds
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = ts.it_value.tv_nsec;  // set reload value, make timer periodic

    if( timer_settime(timer, 0, &ts, NULL) ) {
        logit( "Timer set failed!\n" );
    }

    sem_init(&metrics_sem, 0, 0);

    while(1) {
        sem_wait(&metrics_sem);
        logit( "Counter: %u\n", counter );
    }
    return NULL;
}


