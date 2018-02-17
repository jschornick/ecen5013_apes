/*
  File        : hw3.c
  Description : APES Homework 3

  Multithreaded...

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <stdio.h>
#include <pthread.h>
#include "linked_list.h"

#define LOG_FILE hw3.log

void *tf1( void *data );
void *tf2( void *data );

int main(void)
{
    pthread_t t1, t2;

    // thread struct, thread attrs, func, arg
    if( pthread_create( &t1, NULL, tf1, NULL ) ) {
        printf( "Thread 1 create fail!\n" );
    }

    if( pthread_create( &t2, NULL, tf2, NULL ) ) {
        printf( "Thread 2 create fail!\n" );
    }

    // thread struct, void **retval
    pthread_join( t1, NULL );
    pthread_join( t2, NULL );

    return 0;
}

void *tf1( void *data )
{
    for( int i=0; i<20; i++ ) {
        printf( "Thread 1\n" );
    }
    return NULL;
}

void *tf2( void *data )
{
    for( int i=0; i<20; i++ ) {
        printf( "Thread 2\n" );
    }
    return NULL;
}
