/*
  File        : threads.h
  Description : APES Homework 3

  General threading support function declarations

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#ifndef _THREADS_H
#define _THREADS_H

#include <stdint.h>
#include <stdio.h>

// thread_info_t
//
// This carries all of the unique information relevant to one thread.
//
// It will be prepopulated with some values by the main thread, but most
// information is populated when an individual thread calls thead_init()
//
// Each thread's TLS is configured to point to its thread_info to allow
// convenient access to this information.
//
typedef struct thread_info {
    pthread_t pthread_tid; // The POSIX thread ID
    pid_t linux_tid;       // The kernel thread ID
    uint8_t app_tid;       // Program-specific identifier [from main thread]
    const char *name;      // Human readable thread name
    timer_t timer;         // POSIX timer identifier (for timer-based workers)
    char *log_filename;    // The filename to open and log to [from main thread]
    FILE *p_logfile;       // File handle of the log once opened
} thread_info_t;

typedef enum thread_name {
    THREAD_MAIN = 0,
    THREAD_LETTERS,
    THREAD_METRICS,
    THREAD_MAX
} thread_name_t;

// Global, pre-allocated thread_info structs for all threads
extern thread_info_t thread_info[];

// Thread local storage (TLS) key pointing to our thread_info struct
extern pthread_key_t tinfo_key;

// Human-readable thread names
extern const char *thread_name[];

// Function: init_threading
//
// Initializes the threading structures that will be used to manage worker threads.
//
// This must be called from main before spawning any worker threads.
//
// param[in] log_filename - The name of the log file common to all threads
// returns - A pointer to the thread_info struct for the main thread
//
thread_info_t * init_threading( char *log_filename );


// Function: thread_init
//
// Initializes a single thread.
//
// Finishes populating the thread_info struct for the thread, opens the thread's
// logging file handle, and logs the startup.
//
// param[in,out] tinfo - Pointer to the thread's partially populated thread-info struct
//
void thread_init( thread_info_t *tinfo);


// Function: thread_cleanup
//
// Cleans up an exiting thread. Called as a destructor when TLS is removed.
//
// Resources used by the thread are freed, including log file handle and any
// POSIX timers.
//
// param[in] data - A pointer to the thread_info struct (value of the tinfo TLS)
//
void thread_cleanup(void *data);

#endif /* _THREADS_H */
