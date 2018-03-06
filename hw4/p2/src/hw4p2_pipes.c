/*
  File        : hw4p2_pipes.c
  Description : APES Homework 4, Problem 2

  User space example of the pipe IPC mechanisms.

    Usage: ./hw4p2_pipes

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw4
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "messages.h"

// Standard indexing of pipe file descriptor array
#define PIPE_IN  0
#define PIPE_OUT 1

// Need two pipes since they are one-way only
int pipe_to_parent[2];
int pipe_to_child[2];

// String we'll send as one of our messages
const char test_string[] =  "Pipes are cool!";

void parent(void);
void child(void);


// Function: main
//
// This is the primary program entry point.
//
// It sets up the anonymous pipes so that the file descriptors can be inherited
// by the child, then immediately forks and runs separate functions for each
// process.
//
int main( int argc, char *argv[] )
{

    printf("IPC with Pipes!\n");

    if( pipe(pipe_to_parent) ) {
        printf("Could not create pipe to parent!\n");
        return -1;
    } else {
        printf("Created pipe to parent\n");
    }

    if( pipe(pipe_to_child) ) {
        printf("Could not create pipe to child!\n");
        return -1;
    } else {
        printf("Created pipe to child\n");
    }

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
// It sends an LED message type to the child via a one-way pipe, and prints any
// mesage received through the other pipe.
void parent(void)
{
    printf("Parent has PID: %u\n", getpid());

    // Close ends of pipes we won't read from or write to
    close(pipe_to_parent[PIPE_OUT]);
    close(pipe_to_child[PIPE_IN]);

    // Assign names to the read/write pipes that belong to us
    int read_fd = pipe_to_parent[PIPE_IN];
    int write_fd = pipe_to_child[PIPE_OUT];


    // Build an LED message to send
    msg_t msg;
    msg.header.type = MSG_LED;
    msg.header.data_len = sizeof(msg_led_t);

    msg_led_t msg_data;
    msg_data.on_off = LED_ON;
    msg.data = &msg_data;

    char str_buf[100]; // temporary buffer for string conversions

    // Send message to child through pipe
    printf( "Parent sending: %s\n", msg_to_str(str_buf, &msg) );
    write( write_fd, &msg.header, sizeof(msg_header_t) );
    write( write_fd, msg.data, msg.header.data_len );

    // Read messages child sent through other pipe
    read( read_fd, &msg.header, sizeof(msg_header_t) );
    msg.data = malloc( msg.header.data_len );
    read( read_fd, msg.data, msg.header.data_len );
    printf( "Parent recieved: %s\n", msg_to_str(str_buf, &msg) );
    free( msg.data );
}


// Function: child
//
// This is the main body of code executed by the child after the program forks.
//
// It sends a string message type to the parent via a one-way pipe, and prints
// any mesage received through the other pipe.
void child(void)
{
    printf("Child has PID: %u\n", getpid());

    // Close ends of pipes we won't read from or write to
    close(pipe_to_child[PIPE_OUT]);
    close(pipe_to_parent[PIPE_IN]);

    // Assign names to the read/write pipes that belong to us
    int read_fd = pipe_to_child[PIPE_IN];
    int write_fd = pipe_to_parent[PIPE_OUT];

    msg_t msg;

    // Receive message from parent via pipe
    read( read_fd, &msg.header, sizeof(msg_header_t) );
    msg.data = malloc( msg.header.data_len );
    read( read_fd, msg.data, msg.header.data_len );

    char str_buf[100]; // temporary buffer for string conversion
    printf( "Child recieved: %s\n", msg_to_str(str_buf, &msg) );
    free(msg.data);

    // Build and send a message to parent over other pipe
    msg.header.type = MSG_STRING;
    msg.data = (char *) test_string;
    msg.header.data_len = strlen(test_string) + 1;

    printf( "Child sending: %s\n", msg_to_str(str_buf, &msg) );
    write( write_fd, &msg.header, sizeof(msg_header_t) );
    write( write_fd, msg.data, msg.header.data_len );
}
