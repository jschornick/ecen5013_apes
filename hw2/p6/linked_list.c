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

#include <stdio.h>

#include "linked_list.h"

node_t * insert_at_beginning( node_t *p_head, node_t *p_new )
{
    // No matter what, our new node is the head, so mark it as such
    p_new->prev = NULL;

    // Since we've been passed in the head, mark it as the next node
    // in the chain
    p_new->next = p_head;

    // back link the previous head to the new node if non-null
    if( p_new->next )
    {
        p_new->next->prev = p_new;
    }

    return p_new;
}


node_t * insert_at_end( node_t *p_head, node_t *p_new )
{
    // No matter what, our new node is the tail, so mark it as such
    p_new->next = NULL;

    node_t *p_last = get_tail(p_head);

    // We know p_last was the previous tail, wire it up to the new tail
    p_new->prev = p_last;

    // and the corresponding link back
    if( p_new->prev )
    {
        p_new->prev->next = p_new;
    }

    return p_head;
}

node_t * insert_at_position( node_t *p_base, node_t *p_new, int32_t offset )
{

    node_t *p_node;

    p_node = get_offset( p_base, offset );

    if( p_node ) {
        p_new->next = p_node;
        p_new->prev = p_node->prev;

        if (p_new->prev) {
            p_new->prev->next = p_new;
        }
        p_node->prev = p_new;
    }


    return get_head(p_new);
}


node_t * delete_from_beginning( node_t *p_head )
{
    if( p_head )
    {
        // if there is a node following head, it's now the new head
        if( p_head->next )
        {
            p_head->next->prev = NULL;
        }
        p_head = p_head->next;
    }
    return p_head;
}

node_t * delete_from_end( node_t *p_head )
{

    node_t * p_tail = get_tail(p_head);
    if( p_tail )
    {
        // if there is a node before tail, it's now the new tail
        if( p_tail->prev )
        {
            p_tail->prev->next = NULL;
        }
        p_head = get_head(p_tail);
    }
    return p_head;
}


node_t * delete_from_position( node_t *p_base, int32_t offset )
{
    node_t *p_node;

    p_node = get_offset( p_base, offset );

    if( p_node )
    {
        if( p_node->prev ) {
            p_node->prev->next = p_node->next;
        }
        if( p_node->next ) {
            p_node->next->prev = p_node->prev;
        }
    }

    return get_head(p_node);

}

size_t size( node_t *p_node )
{
    size_t count = 0;
    p_node = get_head(p_node);
    while( p_node ) {
        p_node = p_node->next;
        count++;
    }
    return count;
}

node_t * get_head( node_t *p_node )
{
    node_t *p_last = p_node;
    // walk list backward until at head
    while( p_node )
    {
        p_last = p_node;
        p_node = p_node->prev;
    }

    return p_last;
}

node_t * get_tail( node_t *p_node )
{
    node_t *p_prev = p_node;
    // walk list until at end
    while( p_node )
    {
        p_prev = p_node;
        p_node = p_node->next;
    }

    return p_prev;
}

node_t * get_offset( node_t *p_node, int32_t offset )
{
    if( !offset ) {
        return p_node;
    }
    while( p_node && (offset > 0) )
    {
        p_node = p_node->next;
        offset--;
    }
    while( p_node && (offset < 0) )
    {
        p_node = p_node->prev;
        offset++;
    }
    return p_node;
}
