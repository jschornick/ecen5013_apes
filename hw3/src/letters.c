/*
  File        : letters.c
  Description : APES Homework 3

  Letter counting text analyzer worker definition.

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <stdlib.h>

#include "threads.h"
#include "logging.h"
#include "linked_list.h"
#include "letters.h"

void *letter_counter( void *data )
{
    thread_info_t *tinfo = data;
    thread_init( tinfo );

    FILE *p_file = fopen( LETTER_FILE, "r" );

    if (p_file == NULL) {
        logit( "Failed to open file: '%s'\n", LETTER_FILE );
        return NULL;
    }
    logit( "Successfully opened file: '%s'\n", LETTER_FILE );

    typedef struct count {
        uint32_t count;
        char letter;
        node_t node;
    } count_t;

    node_t *p_head = NULL;
    count_t *p_count;

    for( char ch = 'Z'; ch >= 'A'; ch-- ) {
        p_count = malloc( sizeof(count_t) );
        p_count->letter = ch;
        p_count->count = 0;
        p_head = insert_at_beginning( p_head, &p_count->node );
    }

    int inchar = 0;  // fgetc returns int, and necessary to catch EOF
    while( (inchar = fgetc(p_file)) != EOF ) {
        inchar &= ~0x20;  // convert ASCII to upppercase
        if( (inchar >= 'A') && (inchar <= 'Z') ) {
            p_count = LIST_CONTAINER(get_offset(p_head, inchar - 'A'), count_t, node);
            p_count->count++;
        }
    }
    logit( "Letter counting complete\n" );
    while( p_head != NULL) {
        p_count = LIST_CONTAINER(p_head, count_t, node);
        //logit( "Total for %c : %u\n", p_count->letter, p_count->count );
        if (p_count->count == LETTER_COUNT_TARGET) {
            logit( "Letter '%c' has exactly %d occurances\n", p_count->letter, LETTER_COUNT_TARGET );
        }
        p_head = p_head->next;
    }

    return NULL;
}
