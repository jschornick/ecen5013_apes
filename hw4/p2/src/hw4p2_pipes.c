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

#include "messages.h"

#define PIPE_IN  0
#define PIPE_OUT 1

void parent(void);
void child(void);

int pipe_to_parent[2];
int pipe_to_child[2];

// Function: main
//
// This is the primary program entry point.
//
int main( int argc, char *argv[] )
{

    pipe(pipe_to_parent);
    pipe(pipe_to_child);

    printf("Pipes!\n");

    if( fork() ) {
        parent();
    } else {
        child();
    }

    return 0;
}

void parent(void)
{
    printf("Parent, PID: %u\n", getpid());
    close(pipe_to_parent[PIPE_OUT]);
    close(pipe_to_child[PIPE_IN]);

    int read_fd = pipe_to_parent[PIPE_IN];
    int write_fd = pipe_to_child[PIPE_OUT];

    char str[] = "From parent";
    char buf[100];

    printf( "Parent sending: '%s'\n", str );
    write(write_fd, str, strlen(str) + 1);
    read(read_fd, buf, 100);
    printf( "Parent recieved: '%s'\n", buf );
}

void child(void)
{
    printf("Child, PID: %u\n", getpid());
    close(pipe_to_child[PIPE_OUT]);
    close(pipe_to_parent[PIPE_IN]);

    int read_fd = pipe_to_child[PIPE_IN];
    int write_fd = pipe_to_parent[PIPE_OUT];

    char str[] = "From child";
    char buf[100];

    printf( "Child sending: '%s'\n", str );
    write(write_fd, str, strlen(str) + 1);
    read(read_fd, buf, 100);
    printf( "Child recieved: '%s'\n", buf );
}
