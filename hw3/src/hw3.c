/*
  File        : hw3.c
  Description : APES Homework 3

  Multithreaded program which concurrently analyzes a file reports CPU metrics.

    Usage: ./hw3 [logfile]

  If [logfile] is not provided, a default will be used.

  The program responds to SIGUSR1 or SIGUSR2 for graceful shutdown of worker
  threads. The main thread also accepts these signals and will ensure the
  workers shutdown down before exiting completely.

  NOTE: Graceful shutdown is handled by a hook in the TLS which can be used to
  call a function when the TLS is removed prior to termination.

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <pthread.h>
#include <signal.h>

#include "threads.h"
#include "logging.h"
#include "metrics.h"
#include "letters.h"


void signal_handler(int sig);


// Function: main
//
// This is the primary program entry point, responsible for creating all worker threads
// and ensuring that their work is complete before exiting.
//
int main( int argc, char *argv[] )
{
    char * log_filename;

    // Get log file name from the command line or use a default
    if ( argc < 2 ) {
        log_filename = default_logfile;
    } else {
        log_filename = argv[1];
    }

    // Setup and initialize threading structures that will be used to manage worker threads.
    thread_info_t *tinfo = init_threading( log_filename );

    // Using a simple call to signal() is fine for our requirements
    // All threads will share this signal handler.
    signal( SIGUSR1, signal_handler );
    signal( SIGUSR2, signal_handler );

    pthread_t letters_thread, metrics_thread;

    // Create worker threads
    if( pthread_create( &letters_thread, NULL, letter_counter, &thread_info[THREAD_LETTERS] ) ) {
        logit( "Failed to create letters thread!\n" );
    }

    if( pthread_create( &metrics_thread, NULL, metrics_reporter, &thread_info[THREAD_METRICS] ) ) {
        logit( "Failed to create metrics thread!\n" );
    }

    // Don't exit until all workers have exited
    pthread_join( letters_thread, NULL );
    pthread_join( metrics_thread, NULL );

    logit( "All worker threads joined\n" );

    // Also cleanup main thread before process exits completely
    thread_cleanup(tinfo);

    return 0;
}


// Function: signal_handler
//
// This is the common signal handler used to respond to SIGUSR1 and SIGUSR2 for
// graceful shutdown.
//
void signal_handler(int sig)
{
    thread_info_t *tinfo = pthread_getspecific(tinfo_key);

    if( tinfo->app_tid != THREAD_MAIN ) {
        // Worker thread caught the signal, just cleanup and exit that thread.
        logit( "Caught signal %d, exiting gracefully...\n", sig );
        // NOTE: Thread exit will cause thread_cleanup() to be called as a
        // destructor when each thread's TLS is removed
        pthread_exit(0);
    } else {
        // Main thread caught the signal, consider this an indication that
        // the entire process should shutodwn. Clean up workers accordingly.
        logit( "Caught signal %d, cancelling child threads...\n", sig );
        for(thread_name_t idx = 1; idx<THREAD_MAX; idx++) {
            pthread_cancel(thread_info[idx].pthread_tid);
        }
    }
}
