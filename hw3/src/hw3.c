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
#include <sys/resource.h>  // rusage

#include "linked_list.h"
#include "threads.h"
#include "logging.h"

#include <unistd.h>   // sleep

#define METRICS_INTERVAL_MS 100

void *letter_counter( void *data );
void *metrics_reporter( void *data );
void signal_handler(int sig);

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

    signal( SIGUSR1, signal_handler );
    signal( SIGUSR2, signal_handler );

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

    logit( "All threads joined\n" );
    thread_cleanup(tinfo);

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
    event.sigev_notify = SIGEV_THREAD;   // just call a function, don't signal
    event.sigev_notify_function = timer_func;
    event.sigev_value.sival_ptr = NULL;  // data value passed in when using SIGEV_THREAD?
    event.sigev_notify_attributes = NULL;

    //timer_t timer;
    if (timer_create(CLOCK_MONOTONIC, &event, &tinfo->timer) ) {
        logit( "Timer create failed\n" );
    }

    struct itimerspec ts;
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = METRICS_INTERVAL_MS * 1000000;  // expiration time in nanoseconds
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = ts.it_value.tv_nsec;  // set reload value, make timer periodic

    if( timer_settime(tinfo->timer, 0, &ts, NULL) ) {
        logit( "Timer set failed!\n" );
    }

    struct rusage usage;

    sem_init(&metrics_sem, 0, 0);

    while(1) {
        sem_wait(&metrics_sem);
        getrusage( RUSAGE_SELF, &usage );
        logit( "CPU stats: User %lu.%06lu, Kernel %lu.%06lu, CS(vol) %u, CS(inv) %u\n",
               usage.ru_utime.tv_sec, usage.ru_utime.tv_usec,
               usage.ru_stime.tv_sec, usage.ru_stime.tv_usec,
               usage.ru_nvcsw, usage.ru_nivcsw);

    }
    return NULL;
}


void signal_handler(int sig)
{
    thread_info_t *tinfo = pthread_getspecific(tinfo_key);
    if( tinfo->app_tid != THREAD_MAIN ) {
        logit( "Caught signal %d, exiting gracefully...\n", sig );
        // NOTE: Thread exit will cause thread_cleanup() to be called as a
        // destructor when each thread's TLS is removed
        pthread_exit(0);
    } else {
        logit( "Caught signal %d, cancelling child threads...\n", sig );
        for(thread_name_t idx = 1; idx<THREAD_MAX; idx++) {
            pthread_cancel(thread_info[idx].pthread_tid);
        }
    }
}
