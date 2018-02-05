/*
  File        : hw2_p2.c
  Description : APES Homework 2, Problem 2.
                This is program demonstrates the use of basic file operations.

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw2
 */

#include <stdio.h>   // printf, file functions
#include <stdlib.h>  // malloc
#include <sys/stat.h>  // chmod

#define FILENAME "testfile_hw2_p2.txt"
#define MAX_CHARS 100

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

#define ERR_USAGE (-1)
#define ERR_FILE  (-2)
#define ERR_MEM   (-3)

int main( int argc, char **argv )
{

    FILE *p_file = NULL;

    // If not compiled to read form standad input, require command-line argument
    #ifndef READ_STDIN
    if( argc < 2 )
    {
        printf( "Usage: %s <string-to-print>\n", argv[0]);
        return ERR_USAGE;
    }
    #endif

    // Part 0 : Print a string
    // *************************

    printf("This is an interesting string. Marvel at its fascinating characters.\n");


    // Part 1 : Create a file and change its mode
    // *********************************************

    p_file = fopen( FILENAME, "w+" );
    if( !p_file )
    {
        printf( "Failed to open/create file: %s\n", FILENAME );
        return ERR_FILE;
    }
    fclose( p_file );

    // Change permissions to be read/write for user
    chmod( FILENAME, S_IRUSR | S_IWUSR );


    // Part 2 : Write character to file
    // **********************************

    p_file = fopen( FILENAME, "w+" );  // open for writing, overwrite
    if( !p_file )
    {
        printf( "Failed to open file for writing: %s\n", FILENAME );
        return ERR_FILE;
    }

    fputc( 'Z', p_file );
    fclose( p_file );


    // Part 3 : Read an input string, append, and flush
    // **************************************************

    p_file = fopen( FILENAME, "a" );  // open for append
    if( !p_file )
    {
        printf( "Failed to open file for writing: %s\n", FILENAME );
        return ERR_FILE;
    }

    char *p_buf = NULL;
    p_buf = malloc( sizeof(*p_buf) * (MAX_CHARS+1) ); // characters + null
    if( !p_buf )
    {
        printf( "Failed dynamically allocate array size %u!\n", (MAX_CHARS+1) );
        return ERR_MEM;
    }

    // Read either from stdin or command-line based on how compiled
    #ifdef READ_STDIN
    scanf( "%" STRINGIFY(MAX_CHARS) "s", p_buf); // scanf reads X character + null
    fprintf( p_file, "%s", p_buf );
    #else
    fprintf( p_file, "%s", argv[1] );
    #endif

    fflush( p_file );

    fclose( p_file );


    // Part 4 : Read from file
    // *************************

    p_file = fopen( FILENAME, "r" );  // open for reading
    if( !p_file )
    {
        printf( "Failed to open file for reading: %s\n", FILENAME );
        return ERR_FILE;
    }

    char my_char;
    my_char = fgetc( p_file );
    printf( "Read character: %c\n", my_char );

    fgets( p_buf, (MAX_CHARS+1), p_file ); // fgets reads X-1 chars + null
    if( p_buf )
    {
        printf( "Read string: %s\n", p_buf );
    } else {
        printf( "Failed to read string!\n" );
    }

    fclose( p_file );
    free( p_buf );

    printf( "\n" );

    return 0;

}
