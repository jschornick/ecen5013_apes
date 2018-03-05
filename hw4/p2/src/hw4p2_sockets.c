/*
  File        : hw4p2_sockets.c
  Description : APES Homework 4, Problem 2

  User space example of the socket IPC mechanisms.

    Usage: ./hw4p2_sockets

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>

#include "messages.h"

void parent(void);
void child(void);

#define SOCKET_PATH "/tmp/apes_socket"
#define CLIENT_PATH "/tmp/apes_socket_client"
#define SOCKET_BACKLOG 10
#define BUFFER_SIZE 100

// Function: main
//
// This is the primary program entry point.
//
int main( int argc, char *argv[] )
{

    printf("Sockets!\n");

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

    struct sockaddr_un addr;
    int server_fd;

    server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);  // 0 = default proto
    if( server_fd < 0 ) {
        printf( "Parent failed to create server socket\n" );
        return;
    }
    printf( "Parent created server socket\n" );

    addr.sun_family = AF_UNIX;
    strncpy( addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) );

    unlink( SOCKET_PATH );
    if( bind(server_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1 ) {
        printf( "Parent failed to bind server socket: %s\n", strerror(errno) );
        return;
    }
    printf( "Parent bound server socket: %s\n", addr.sun_path );

    ssize_t count;
    char buffer[BUFFER_SIZE];
    struct sockaddr_un client_addr;

    socklen_t socklen;

    socklen = sizeof(client_addr);

    if( (count = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &client_addr, &socklen)) == -1) {
        printf( "Parent receive failed\n" );
        return;
    }
    printf( "Parent socket received: '%s'\n", buffer );

    strcpy( buffer, "From parent" );
    sendto( server_fd, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &client_addr, socklen );

    close( server_fd );
    unlink( SOCKET_PATH );
}


void child(void)
{
    printf("Child, PID: %u\n", getpid());

    struct sockaddr_un client_addr;
    int sock_fd;

    // create client end of socket (required for SOCK_DGRAM)
    sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);  // 0 = default proto
    if( sock_fd < 0 ) {
        printf( "Child failed to create socket\n" );
        return;
    }
    printf( "Child created socket\n" );

    client_addr.sun_family = AF_UNIX;
    strncpy( client_addr.sun_path, CLIENT_PATH, sizeof(client_addr.sun_path) );
    unlink( CLIENT_PATH );
    if( bind(sock_fd, (struct sockaddr *) &client_addr, sizeof(client_addr)) == -1 ) {
        printf( "Child failed to bind client socket: %s\n", strerror(errno) );
        return;
    }
    printf( "Child bound client socket: %s\n", client_addr.sun_path );

    // server side
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strncpy( server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) );

    ssize_t len;
    char buffer[BUFFER_SIZE];
    strcpy( buffer, "From child" );

    len = strlen(buffer)+1;

    while( sendto(sock_fd, buffer, len, 0, (struct sockaddr *) &server_addr, sizeof(server_addr)) != len ) {
        if( errno != ENOENT ) {
            printf( "Child failed to send message to server socket: %s\n", strerror(errno) );
            return;
        }
        printf( "Child waiting for server...\n" );
        usleep(10000);
    }

    if( (len = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, NULL, NULL)) == -1 ) {
        printf( "Child failed to receive from receive failed\n" );
        return;
    }
    printf( "Child received : '%s'\n", buffer );

    close( sock_fd );
    unlink( CLIENT_PATH );
}
