/*
  File        : hw2_p3.c
  Description : APES Homework 2, Problem 3.

  This is program tests the new sort syscall.

  A source buffer is filled with random integers, then passed to the kernel
  via sys_sort() along with a second buffer to store the results. After the
  system call, the results are validated for correctness.

  Additional tests are performed to verify that the system call correctly
  handles invalid pointers and buffer sizes.

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw2
 */

#include <stdio.h>   // printf, file functions
#include <stdint.h>
#include <stdlib.h>  // rand
#include <time.h>    // srand

// indirect syscall support, requires _GNU_SOURCE to be defined
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

#define SYS_SORT 333
#define BUF_SIZE 256

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


/* Function : check_sort
 *
 * Checks that a particular buffer has been sorted correctly and reports
 * the result.
 */
void check_sort( int32_t *p_buf);
void check_sort( int32_t *p_buf)
{
    uint8_t correct = 1;
    for( size_t idx = 0; idx < BUF_SIZE-1; idx++ )
    {
        if ( p_buf[idx] < p_buf[idx+1] )
        {
            correct = 0;
        }
    }
    if ( correct )
    {
        printf( "Order is correct\n");
    }
    else
    {
        printf( "Order is NOT correct\n");
    }
}


int main( void )
{
    printf( "APES Homework 2, Problem 3\n" );
    printf( "Testing new sort system call...\n" );

    long retval;

    int32_t unsorted[BUF_SIZE];
    int32_t sorted[BUF_SIZE];

    srand(time(NULL));

    for ( uint8_t test = 0; test < TEST_COUNT; test++ )
    {
        printf( "Valid param test # %02u : ", test );
        rand_fill( unsorted, BUF_SIZE );
        rand_fill( sorted, BUF_SIZE );

        retval = syscall( SYS_SORT, unsorted, sorted, BUF_SIZE );
        printf( "sys_sort() = %ld : ", retval );
        check_sort( sorted );
    }

    printf( "Invalid src test : " );
    rand_fill( unsorted, BUF_SIZE );
    rand_fill( sorted, BUF_SIZE );
    retval = syscall( SYS_SORT, 23, sorted, BUF_SIZE );
    printf( "sys_sort() = %ld : errno = %d\n", retval, errno );

    printf( "Invalid dst test : " );
    rand_fill( unsorted, BUF_SIZE );
    rand_fill( sorted, BUF_SIZE );
    retval = syscall( SYS_SORT, unsorted, 42, BUF_SIZE );
    printf( "sys_sort() = %ld : errno = %d\n", retval, errno );

    printf( "Size too large test: " );
    rand_fill( unsorted, BUF_SIZE );
    rand_fill( sorted, BUF_SIZE );
    retval = syscall( SYS_SORT, unsorted, sorted, 2<<30 );
    printf( "sys_sort() = %ld : errno = %d\n", retval, errno );

    return retval;
}
