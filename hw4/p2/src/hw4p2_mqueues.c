/*
  File        : hw4p2_mqueues.c
  Description : APES Homework 4, Problem 2

  User space example of the message queue IPC mechanisms.

    Usage: ./hw4p2_queues

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw4
*/

#include <stdio.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>  // O_* defines
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "messages.h"

// Parameters which define our message queue limits
#define MAX_MSG_SIZE 100
#define MAX_MSG_COUNT 10

// We'll use one queue for receiving messages for each process
#define MQ_NAME_P "/parent_mq"  /* messages for parent */
#define MQ_NAME_C "/child_mq"   /* messages for child */

// String we'll send as one of our messages
const char test_string[] =  "Message queues are cool!";

void parent(void);
void child(void);


// Function: main
//
// This is the primary program entry point.
//
// It immediately forks and runs a separate function for each process.
//
int main( int argc, char *argv[] )
{

    printf("IPC with Queues!\n");

    if( fork() ) {
        parent();
    } else {
        child();
    }

    return 0;
}


// Function: parent
//
// This is the main body of code executed by the parent after the program forks.
//
// It places an LED message type into the child's message queue, and prints any
// message received through it's own separate message queue.
void parent(void)
{
    printf("Parent has PID: %u\n", getpid());

    mqd_t mq_c;  // messages to child
    mqd_t mq_p;  // messages for parent
    struct mq_attr attr;

    attr.mq_maxmsg = MAX_MSG_COUNT;   // max number of messages
    attr.mq_msgsize = MAX_MSG_SIZE;  // max messages size (bytes)

    // Create our queue where we can receive messages.
    mq_p = mq_open( MQ_NAME_P, O_CREAT | O_RDONLY, 0666, &attr );
    if( mq_p == (mqd_t) -1 ) {
        printf( "Parent failed to open own MQ: %d\n", errno );
        return;
    }
    printf( "Parent created queue\n" );

    // Attempt to open child's message queue. The child is responsible for creating
    // this queue, so retry if necessary.
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


    // Buffer we'll use for serializing the message structure into a single
    // message on the message queue.
    void *msg_buf = malloc(MAX_MSG_SIZE);
    size_t msg_buf_len;

    // Build an LED message to send
    msg_t msg;
    msg.header.type = MSG_LED;
    msg.header.data_len = sizeof(msg_led_t);

    msg_led_t msg_data;
    msg_data.on_off = LED_ON;
    msg.data = &msg_data;

    char str_buf[100]; // temporary buffer for string conversion

    // Place serialized message in child's message queue
    printf( "Parent sending: %s\n", msg_to_str(str_buf, &msg) );
    msg_buf_len = msg_to_msgbuf(msg_buf, &msg);
    if( mq_send( mq_c, msg_buf, msg_buf_len, 0) ) {
        printf( "Parent failed to send: %s!\n", strerror(errno) );
    }

    // Read any message received in our message queue
    if( mq_receive( mq_p, msg_buf, MAX_MSG_SIZE, NULL) < 0 ) {
        printf( "Parent failed to receive: %d\n", errno );
    } else {
        msgbuf_to_msg( &msg, msg_buf );
        printf( "Parent recieved: %s\n", msg_to_str(str_buf, &msg) );
    }

    mq_close( mq_c );
    mq_close( mq_p );
    mq_unlink( MQ_NAME_P );
}


// Function: child
//
// This is the main body of code executed by the child after the program forks.
//
// It places a string message type into the parent's message queue, and prints any
// mesage received through it's own separate message queue.
void child(void)
{
    printf("Child has PID: %u\n", getpid());

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


    // Buffer we'll use for serializing the message structure into a single
    // message on the message queue.
    void *msg_buf = malloc(MAX_MSG_SIZE);
    size_t msg_buf_len;

    // Build a string message to send
    msg_t msg;
    msg.header.type = MSG_STRING;
    msg.data = (char *) test_string;
    msg.header.data_len = strlen(test_string) + 1;

    char str_buf[100]; // temporary buffer for string conversion

    // Place serialized message in parent's message queue
    printf( "Child sending: %s\n", msg_to_str(str_buf, &msg) );
    msg_buf_len = msg_to_msgbuf(msg_buf, &msg);
    if( mq_send( mq_p, msg_buf, msg_buf_len, 0) ) {
        printf( "Child failed to send: %d!\n", errno );
    }

    // Read any message received in our message queue
    if( mq_receive( mq_c, msg_buf, MAX_MSG_SIZE, NULL) < 0 ) {
        printf( "Child failed to receive: %d\n", errno );
    } else {
        msgbuf_to_msg( &msg, msg_buf );
        printf( "Child received: '%s'\n", msg_to_str(str_buf, &msg) );
    }

    mq_close( mq_p );
    mq_close( mq_c );
    mq_unlink( MQ_NAME_C );
}
