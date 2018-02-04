/*
  File        : hw2_p4.c
  Description : APES Homework 2, Problem 4.

  This is program calls the new sort syscall to sort a buffer, plus a few
  existing syscalls to collect system/process information.

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw2
 */

#include <stdio.h>   // printf, file functions
#include <stdint.h>
#include <stdlib.h>  // rand
#include <time.h>    // srand, timeval, strftime, localtime

#include <unistd.h>  // getpid, getuid
#include <errno.h>
#include <sys/types.h>   // pid_t, uid_t
#include <sys/syscall.h> // indirect syscall support, requires _GNU_SOURCE to be defined
#include <sys/time.h>    // gettimeofday


#define SYS_SORT 333
#define BUF_SIZE 256
#define TIME_BUF_SIZE 50

#define TEST_COUNT 10


/* Function : rand_fill
 *
 * This function fills a buffer of the specified size with random
 * integers from 0 to RAND_MAX, which is 2^31-1 on most systems
 */
void rand_fill( int32_t *p_buf, size_t buf_size );
void rand_fill( int32_t *p_buf, size_t buf_size )
{
    for( size_t idx = 0; idx < buf_size; idx++ ) {
        p_buf[idx] = rand();  // [0, RAND_MAX] == [0, 2^31-1]
    }
}


int main( void )
{
    printf( "---\n" );
    printf( "APES Homework 2, Problem 4\n" );

    long retval;

    struct timeval time_now;
    retval = gettimeofday(&time_now, NULL);

    char time_buf[TIME_BUF_SIZE];
    printf( "Syscall gettimeofday() returned: %ld\n", retval );
    if( !retval )
    {
        // Since we the syscall returned success, convert the populated timeval into
        // a tm structure that can be formatted with strftime
        strftime(time_buf, TIME_BUF_SIZE, "%Y/%m/%d %T", localtime(&time_now.tv_sec) );
        printf( "Current time/date : %s\n", time_buf );
    }

    pid_t pid;
    pid = getpid();
    printf( "Syscall getpid() returned: %d\n", pid );

    uid_t uid;
    uid = getuid();
    printf( "Syscall getuid() returned: %d\n", uid );


    int32_t unsorted[BUF_SIZE];
    int32_t sorted[BUF_SIZE];

    srand(time(NULL));
    rand_fill( unsorted, BUF_SIZE );
    retval = syscall( SYS_SORT, unsorted, sorted, BUF_SIZE );

    printf( "Syscall sort() returned: %ld\n", retval );

}
