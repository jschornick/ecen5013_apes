/*
  File        : hw4p2_pipes.c
  Description : APES Homework 4, Problem 2

  User space example of the pipe IPC mechanisms.

    Usage: ./hw4p2_pipes

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "messages.h"

#define PIPE_IN  0
#define PIPE_OUT 1

void parent(void);
void child(void);

int pipe_to_parent[2];
int pipe_to_child[2];

const char child_response[] =  "Got your message!";

// Function: main
//
// This is the primary program entry point.
//
int main( int argc, char *argv[] )
{

    pipe(pipe_to_parent);
    pipe(pipe_to_child);

    printf("IPC with Pipes!\n");

    if( fork() ) {
        parent();
    } else {
        child();
    }

    return 0;
}

void parent(void)
{
    printf("Parent has PID: %u\n", getpid());
    close(pipe_to_parent[PIPE_OUT]);
    close(pipe_to_child[PIPE_IN]);

    int read_fd = pipe_to_parent[PIPE_IN];
    int write_fd = pipe_to_child[PIPE_OUT];

    msg_t msg;
    char str_buf[100]; // temporary buffer for string conversion

    // Send LED message to child
    msg.header.type = MSG_LED;
    msg.header.data_len = sizeof(msg_led_t);
    msg_led_t msg_data;
    msg_data.on_off = LED_ON;
    msg.data = &msg_data;

    printf( "Parent sending: %s\n", msg_to_str(str_buf, &msg) );
    write( write_fd, &msg.header, sizeof(msg_header_t) );
    write( write_fd, msg.data, msg.header.data_len );


    // Read response from child
    read(read_fd, &msg.header, sizeof(msg_header_t));
    msg.data = malloc(msg.header.data_len);
    read(read_fd, msg.data, msg.header.data_len);
    printf( "Parent recieved: %s\n", msg_to_str(str_buf, &msg) );
    free(msg.data);
}

void child(void)
{
    printf("Child has PID: %u\n", getpid());
    close(pipe_to_child[PIPE_OUT]);
    close(pipe_to_parent[PIPE_IN]);

    int read_fd = pipe_to_child[PIPE_IN];
    int write_fd = pipe_to_parent[PIPE_OUT];

    msg_t msg;
    char str_buf[100]; // temporary buffer for string conversion

    // Receive message from parent
    read(read_fd, &msg.header, sizeof(msg_header_t));
    msg.data = malloc(msg.header.data_len);
    read(read_fd, msg.data, msg.header.data_len);

    printf( "Child recieved: %s\n", msg_to_str(str_buf, &msg) );
    free(msg.data);

    // Send a response back to parent
    msg.header.type = MSG_STRING;
    msg.data = (char *) child_response;
    msg.header.data_len = strlen(child_response) + 1;

    printf( "Child sending: %s\n", msg_to_str(str_buf, &msg) );
    write( write_fd, &msg.header, sizeof(msg_header_t) );
    write( write_fd, msg.data, msg.header.data_len );

}
