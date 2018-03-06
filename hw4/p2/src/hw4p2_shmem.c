/*
  File        : hw4p2_shmem.c
  Description : APES Homework 4, Problem 2

  User space example of the shared memory IPC mechanisms.

    Usage: ./hw4p2_shmem

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw4
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>  // O_* defines
#include <semaphore.h>
#include <string.h>
#include <errno.h>

#include "messages.h"

// We'll define two POSIX shared memory segments to act as mailboxes
// for the messages delivered to each process.
#define SHM_SIZE 1024
#define SHM_TO_CHILD "/shared_to_child"
#define SHM_TO_PARENT "/shared_to_parent"

// The semaphore names we'll use to signal that messages have been delivered
#define SEM_TO_CHILD "/sem_to_child"
#define SEM_TO_PARENT "/sem_to_parent"

const char test_string[] =  "Shared memory is cool!";

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

    printf("IPC with Shared Memory!\n");

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
// It places an LED message type into the shared memory designated as the
// child's mailbox. It then waits for a message to be delivered to its own
// shared memory location.
//
void parent(void)
{
    printf("Parent, PID: %u\n", getpid());

    int shm_fd;  // temporary shared memory file descriptor prior to mapping

    // The two shared memory segments we we use as mailboxes. We receive messages
    // at the read addres, and send to the write address.
    void *read_shm;
    void *write_shm;

    // Create two semaphores for signalling between the processes.
    sem_t *read_sem;   // Becomes available when a message is present
    sem_t *write_sem;  // Increased when a message is posted

    write_sem = sem_open(SEM_TO_CHILD, O_CREAT, 0666, 0);
    read_sem = sem_open(SEM_TO_PARENT, O_CREAT, 0666, 0);

    // Map the shared memory segments representing the two mailboxes
    shm_fd = shm_open( SHM_TO_PARENT, O_CREAT | O_RDONLY, 0666 );
    if( shm_fd < 0 ) {
        printf( "Parent failed to create SHM\n" );
        return;
    }
    ftruncate( shm_fd, SHM_SIZE);
    read_shm = mmap( NULL, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0 );
    if ( read_shm == MAP_FAILED ) {
        printf( "Parent failed to map readable SHM\n" );
        return;
    }
    close( shm_fd );  // mapped, no longer need descriptor
    printf( "Parent mapped readable SHM @%p\n", read_shm );

    shm_fd = shm_open( SHM_TO_CHILD, O_RDWR | O_CREAT, 0666 );
    if( shm_fd < 0 ) {
        printf( "Parent failed to open SHM: %s\n", strerror(errno) );
        return;
    }
    ftruncate( shm_fd, SHM_SIZE);
    write_shm = mmap( NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0 );
    if ( write_shm == MAP_FAILED ) {
        printf( "Parent failed to map writeable SHM: %s\n", strerror(errno) );
        return;
    }
    close( shm_fd );
    printf( "Parent mapped writeable SHM @%p\n", write_shm );


    // Shared memory has been successfully setup.
    // Now build a message and deliver it to the child's mailbox.
    msg_t msg;
    msg.header.type = MSG_LED;
    msg.header.data_len = sizeof(msg_led_t);
    msg_led_t msg_data;
    msg_data.on_off = LED_ON;
    msg.data = &msg_data;

    char str_buf[100]; // temporary buffer for string conversion

    printf( "Parent sending: %s\n", msg_to_str(str_buf, &msg) );
    msg_to_msgbuf(write_shm, &msg);

    sem_post( write_sem );

    ///

    printf( "Parent waiting for message\n" );
    sem_wait( read_sem );
    msgbuf_to_msg( &msg, read_shm);
    printf( "Parent got message: %s\n", msg_to_str(str_buf, &msg) );

    shm_unlink( SHM_TO_PARENT );
    shm_unlink( SHM_TO_CHILD );

    sem_unlink( SEM_TO_CHILD );
    sem_unlink( SEM_TO_PARENT );
}


// Function: child
//
// This is the main body of code executed by the parent after the program forks.
//
// It places an string message type into the shared memory designated as the
// parents's mailbox. It then waits for a message to be delivered to its own
// shared memory location.
//
void child(void)
{
    printf("Child, PID: %u\n", getpid());

    int shm_fd;  // temporary shared memory file descriptor prior to mapping

    // The two shared memory segments we we use as mailboxes. We receive messages
    // at the read addres, and send to the write address.
    void *read_shm;
    void *write_shm;

    // Create two semaphores for signalling between the processes.
    sem_t *read_sem;
    sem_t *write_sem;

    write_sem = sem_open(SEM_TO_PARENT, O_CREAT, 0666, 0);
    read_sem = sem_open(SEM_TO_CHILD, O_CREAT, 0666, 0);

    // Map the shared memory segments representing the two mailboxes
    shm_fd = shm_open( SHM_TO_CHILD, O_CREAT | O_RDONLY, 0666 );
    if( shm_fd < 0 ) {
        printf( "Child failed to create SHM\n" );
    }
    ftruncate( shm_fd, SHM_SIZE);
    read_shm = mmap( NULL, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0 );
    if ( read_shm == MAP_FAILED ) {
        printf( "Child failed to map readable SHM\n" );
        return;
    }
    close( shm_fd );  // mapped, no longer need descriptor
    printf( "Child mapped readable SHM @%p\n", read_shm );

    shm_fd = shm_open( SHM_TO_PARENT, O_RDWR | O_CREAT, 0666 );
    if( shm_fd < 0 ) {
        printf( "Child failed to open SHM\n" );
    }
    ftruncate( shm_fd, SHM_SIZE);
    write_shm = mmap( NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0 );
    if ( write_shm == MAP_FAILED ) {
        printf( "Child failed to map writeable SHM\n");
        return;
    }
    close( shm_fd );
    printf( "Child mapped writeable SHM @%p\n", write_shm );


    // Shared memory has been successfully setup.
    // Wait until a message is available in our mailbox
    msg_t msg;
    char str_buf[100]; // temporary buffer for string conversion

    sem_wait( read_sem );
    msgbuf_to_msg( &msg, read_shm);
    printf( "Child got message: %s\n", msg_to_str(str_buf, &msg) );


    // Now build a string message and place it in the parent's mailbox.
    msg.header.type = MSG_STRING;
    msg.data = (char *) test_string;
    msg.header.data_len = strlen(test_string) + 1;

    printf( "Child sending: %s\n", msg_to_str(str_buf, &msg) );
    msg_to_msgbuf(write_shm, &msg);
    sem_post( write_sem );

    shm_unlink( SHM_TO_CHILD );
    shm_unlink( SHM_TO_PARENT );

    sem_unlink( SEM_TO_CHILD );
    sem_unlink( SEM_TO_PARENT );
}
