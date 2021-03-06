/*
  File        : hw4p2_sockets.c
  Description : APES Homework 4, Problem 2

  User space example of the socket IPC mechanisms.

    Usage: ./hw4p2_sockets

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw4
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "messages.h"

// Parameters which define our socket communication.
//
// We're using Unix-domain sockets, so a file path is required.
#define SOCKET_PATH "/tmp/apes_socket"
#define CLIENT_PATH "/tmp/apes_socket_client"
#define BUFFER_SIZE 100

// String we'll send as one of our messages
const char test_string[] =  "Sockets are cool!";

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

    printf("IPC with Sockets!\n");

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
// It creates a listening Unix-domain datagram socket, and prints any message
// received over that socket. Once a client message is received, it sends an LED
// message as a response over the associated client socket.
//
void parent(void)
{
    printf("Parent has PID: %u\n", getpid());

    struct sockaddr_un addr;
    int server_fd;

    // The parent acts as the server, so create a listening socket.
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


    // Now that the server side is ready, wait for client socket connections
    struct sockaddr_un client_addr;
    socklen_t socklen = sizeof(client_addr);

    void *msg_buf = malloc(BUFFER_SIZE);
    size_t msg_buf_len;

    if( recvfrom(server_fd, msg_buf, BUFFER_SIZE, 0, (struct sockaddr *) &client_addr, &socklen) == -1) {
        printf( "Parent receive failed\n" );
        return;
    }

    // Deserialize the received message
    msg_t msg;
    char str_buf[100]; // temporary buffer for string conversion
    msgbuf_to_msg( &msg, msg_buf );
    printf( "Parent socket received: %s\n", msg_to_str(str_buf, &msg) );


    // Send an LED message as a response to the client
    msg.header.type = MSG_LED;
    msg.header.data_len = sizeof(msg_led_t);
    msg_led_t msg_data;
    msg_data.on_off = LED_ON;
    msg.data = &msg_data;

    printf( "Parent sending: %s\n", msg_to_str(str_buf, &msg) );
    msg_buf_len = msg_to_msgbuf(msg_buf, &msg);
    sendto( server_fd, msg_buf, msg_buf_len, 0, (struct sockaddr *) &client_addr, socklen );

    close( server_fd );
    unlink( SOCKET_PATH );
}


// Function: child
//
// This is the main body of code executed by the child after the program forks.
//
// It sends a string message type to the parent's listening socket. It then
// receives an arbitrary message type as a response throught the socket connection.
//
void child(void)
{
    printf("Child has PID: %u\n", getpid());

    struct sockaddr_un client_addr;
    int sock_fd;

    // Create client end of socket (required for SOCK_DGRAM socket type)
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

    // Define the server-side socket we want to connect to
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strncpy( server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) );

    void *msg_buf = malloc(BUFFER_SIZE);
    size_t msg_buf_len;
    msg_t msg;
    char str_buf[100]; // temporary buffer for string conversion

    // Build a string type message to send to the server
    msg.header.type = MSG_STRING;
    msg.data = (char *) test_string;
    msg.header.data_len = strlen(test_string) + 1;

    // Send the message to the server's waiting socket. We may need to retry if the server isn't ready yet.
    printf( "Child sending: %s\n", msg_to_str(str_buf, &msg) );
    msg_buf_len = msg_to_msgbuf(msg_buf, &msg);
    while( sendto(sock_fd, msg_buf, msg_buf_len, 0, (struct sockaddr *) &server_addr, sizeof(server_addr)) != msg_buf_len ) {
        if( errno != ENOENT ) {
            printf( "Child failed to send message to server socket: %s\n", strerror(errno) );
            return;
        }
        printf( "Child waiting for server...\n" );
        usleep(10000);
    }

    // Now that we've sent a message, read the server's response mesage
    if( (msg_buf_len = recvfrom(sock_fd, msg_buf, BUFFER_SIZE, 0, NULL, NULL)) == -1 ) {
        printf( "Child failed to receive from receive failed\n" );
        return;
    }
    msgbuf_to_msg( &msg, msg_buf );
    printf( "Child received : %s\n", msg_to_str(str_buf, &msg) );

    close( sock_fd );
    unlink( CLIENT_PATH );
}
