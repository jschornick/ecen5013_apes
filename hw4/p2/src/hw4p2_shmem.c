/*
  File        : hw4p2_shmem.c
  Description : APES Homework 4, Problem 2

  User space example of the shared memory IPC mechanisms.

    Usage: ./hw4p2_shmem

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>  // O_* defines
#include <semaphore.h>  // O_* defines
#include <string.h>  // O_* defines
#include <errno.h>  // O_* defines

#include "messages.h"

#define SHM_SIZE 1024
#define SHM_TO_CHILD "/shared_to_child"
#define SHM_TO_PARENT "/shared_to_parent"

#define SEM_TO_CHILD "/sem_to_child"
#define SEM_TO_PARENT "/sem_to_parent"

void parent(void);
void child(void);


typedef struct shm_msg {
    size_t len;
    void *data;
} shm_msg_t;


// Function: main
//
// This is the primary program entry point.
//
int main( int argc, char *argv[] )
{

    printf("Shared Memory!\n");

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

    int shm_fd;

    void *read_shm;
    void *write_shm;

    sem_t *read_sem;
    sem_t *write_sem;

    write_sem = sem_open(SEM_TO_CHILD, O_CREAT, 0666, 0);
    read_sem = sem_open(SEM_TO_PARENT, O_CREAT, 0666, 0);

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


    shm_msg_t *msg = (shm_msg_t *) write_shm;

    const char str[] = "From parent";
    strcpy( (char* ) &msg->data, str );
    msg->len = strlen(str)+1;
    sem_post( write_sem );
    printf( "Parent wrote message to SHM\n" );

    printf( "Parent waiting for message\n" );
    sem_wait( read_sem );
    msg = (shm_msg_t *) read_shm;
    printf( "Parent got message len: '%lu'\n", msg->len );
    printf( "Parent got message: '%s'\n", (char *) &msg->data );

    shm_unlink( SHM_TO_PARENT );
    shm_unlink( SHM_TO_CHILD );

    sem_unlink( SEM_TO_CHILD );
    sem_unlink( SEM_TO_PARENT );
}


void child(void)
{
    printf("Child, PID: %u\n", getpid());

    int shm_fd;

    void *read_shm;
    void *write_shm;

    sem_t *read_sem;
    sem_t *write_sem;

    write_sem = sem_open(SEM_TO_PARENT, O_CREAT, 0666, 0);
    read_sem = sem_open(SEM_TO_CHILD, O_CREAT, 0666, 0);

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

    sem_wait( read_sem );
    shm_msg_t *msg = (shm_msg_t *) read_shm;
    printf( "Child got message length: '%lu'\n", msg->len );
    printf( "Child got message: '%s'\n", (char *) &msg->data );

    msg = (shm_msg_t *) write_shm;

    const char str[] = "From child";
    strcpy( (char *) &msg->data, str );
    msg->len = strlen(str)+1;
    sem_post( write_sem );
    printf( "Child wrote message to SHM\n" );

    shm_unlink( SHM_TO_CHILD );
    shm_unlink( SHM_TO_PARENT );

    sem_unlink( SEM_TO_CHILD );
    sem_unlink( SEM_TO_PARENT );
}
