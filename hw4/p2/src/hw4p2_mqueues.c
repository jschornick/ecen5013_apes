/*
  File        : hw4p2_mqueues.c
  Description : APES Homework 4, Problem 2

  User space example of the message queue IPC mechanisms.

    Usage: ./hw4p2_queues

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <stdio.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>  // O_* defines
#include <errno.h>
#include <string.h>

#include "messages.h"

void parent(void);
void child(void);

#define MAX_MSG_SIZE 100
#define MAX_MSG_COUNT 10

// Function: main
//
// This is the primary program entry point.
//
int main( int argc, char *argv[] )
{

    printf("Queues!\n");

    if( fork() ) {
        parent();
    } else {
        child();
    }

    return 0;
}

#define MQ_NAME_P "/parent_mq"  /* messages for parent */
#define MQ_NAME_C "/child_mq"   /* messages for child */

void parent(void)
{
    printf("Parent, PID: %u\n", getpid());

    mqd_t mq_c;  // messages to child
    mqd_t mq_p;  // messages for parent
    struct mq_attr attr;

    attr.mq_maxmsg = MAX_MSG_COUNT;   // max number of messages
    attr.mq_msgsize = MAX_MSG_SIZE;  // max messages size (bytes)


    // Open queues

    mq_p = mq_open( MQ_NAME_P, O_CREAT | O_RDONLY, 0666, &attr );
    if( mq_p == (mqd_t) -1 ) {
        printf( "Parent failed to open own MQ: %d\n", errno );
        return;
    }
    printf( "Parent created queue\n" );

    mq_c = mq_open( MQ_NAME_C, O_WRONLY, 066, &attr );
    while( mq_c == (mqd_t) -1 ) {
        if ( errno == ENOENT ) {
        printf( "Parent waiting for child's MQ...\n" );
        mq_c = mq_open( MQ_NAME_C, O_WRONLY, 066, &attr );
        usleep(1000);
        } else {
            printf( "Parent failed to open child's MQ: %s\n", strerror(errno) );
        }
    }
    printf( "Parent opened child queue\n" );


    // Send/receive messages

    char str[MAX_MSG_SIZE];
    strcpy( str, "From parent" );

    if( mq_send( mq_c, str, strlen(str)+1, 0) ) {
        printf( "Parent failed to send: %s!\n", strerror(errno) );
    } else {
        printf( "Parent sent '%s'\n", str );
    }

    if( mq_receive( mq_p, str, MAX_MSG_SIZE, NULL) < 0 ) {
        printf( "Parent failed to receive: %d\n", errno );
    } else {
        printf( "Parent received: '%s'\n", str );
    }

    mq_close( mq_c );
    mq_close( mq_p );
    mq_unlink( MQ_NAME_P );
}


void child(void)
{
    printf("Child, PID: %u\n", getpid());

    mqd_t mq_c;  // messages for child
    mqd_t mq_p;  // messages to parent

    struct mq_attr attr;
    attr.mq_maxmsg = MAX_MSG_COUNT;   // max number of messages
    attr.mq_msgsize = MAX_MSG_SIZE;  // max messages size (bytes)

    mq_c = mq_open( MQ_NAME_C, O_CREAT | O_RDONLY, 0666, &attr );
    if( mq_c == (mqd_t) -1 ) {
        printf( "Child failed to open own MQ: %s\n", strerror(errno) );
        return;
    }
    printf( "Child queue created\n" );

    mq_p = mq_open( MQ_NAME_P, O_WRONLY, 0666, &attr );
    while( mq_p == (mqd_t) -1 ) {
        if ( errno == ENOENT ) { 
            printf( "Child waiting for parent's MQ...\n" );
            mq_p = mq_open( MQ_NAME_C, O_WRONLY, 066, &attr );
            usleep(1000);
        } else {
            printf( "Child failed to open parent's MQ: %s\n", strerror(errno) );
        }
    }
    printf( "Child opened parent queue\n" );
    //sleep(1);

    /////

    char str[MAX_MSG_SIZE];
    strcpy( str, "From child" );

    if( mq_send( mq_p, str, strlen(str)+1, 0) ) {
        printf( "Child failed to send: %d!\n", errno );
    } else {
        printf( "Child sent '%s'\n", str );
    }

    if( mq_receive( mq_c, str, MAX_MSG_SIZE, NULL) < 0 ) {
        printf( "Child failed to receive: %d\n", errno );
    } else {
        printf( "Child received: '%s'\n", str );
    }

    mq_close( mq_p );
    mq_close( mq_c );
    mq_unlink( MQ_NAME_C );
}
