/*
  File        : linked_list.c
  Description : APES Homework 2, Problem 6.

  Doubly-linked list data structure library implementation.

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw2
*/

#include <stdlib.h>  // malloc
#include <stddef.h>
#include "linked_list.h"

node_t * insert_at_beginning( node_t *p_head, node_t *p_new )
{
    // No matter what, our new node is the head, so mark it as such
    p_new->prev = NULL;

    // Since we've been passed in the head, use it as the next node
    // in the chain
    p_new->next = p_head;

    return p_new;
}
