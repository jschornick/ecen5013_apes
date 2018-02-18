/*
  File        : threads.h
  Description : APES Homework 3

  General threading support functions

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
  https://github.com/jschornick/ecen5013_apes/hw3
*/

#ifndef _THREADS_H
#define _THREADS_H

#include <stdint.h>
#include <stdio.h>

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

extern thread_info_t thread_info[];
extern pthread_key_t tinfo_key;

extern const char *thread_name[];

void thread_init( thread_info_t *tinfo);
void thread_cleanup(void *data);

#endif /* _THREADS_H */
