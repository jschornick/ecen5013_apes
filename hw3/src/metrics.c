/*
  File        : metrics.c
  Description : APES Homework 3

  CPU utilization metrics reporter worker definition.

  The worker sleeps between reports by using a combination of POSIX timer and sempahore.

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <stdint.h>
#include <sys/resource.h> // getrusage()
#include <signal.h>       // sigevent for posix timer
#include <semaphore.h>
#include <time.h>

#include "threads.h"
#include "logging.h"
#include "metrics.h"

sem_t metrics_sem;

// This is the timer callback which allows the worker to sleep (on sem_wait) between reports.
// It does only enough work to signal that the timer has expired.
//
// Using the POSIX timer SIGEV_THREAD mechanism, this will run in a separate thread.
void metrics_timer_cb( union sigval val )
{
    // As soon as we increment (unlock) this semaphore, the waiting thread will wake.
    sem_post(&metrics_sem);
}

void *metrics_reporter( void *data )
{
    thread_info_t *tinfo = data;
    thread_init( tinfo );

    struct sigevent event;
    event.sigev_notify = SIGEV_THREAD;   // just call a function, don't signal
    event.sigev_notify_function = metrics_timer_cb;
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

    logit( "Metrics reporter initialized\n" );
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
