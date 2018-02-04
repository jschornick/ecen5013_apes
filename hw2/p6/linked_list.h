/*
  File        : linked_list.h
  Description : APES Homework 2, Problem 6.

  Doubly-linked list data structure library, structures and declarations.

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw2
*/

#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include <stddef.h>  // offsetof

typedef struct node node_t;
typedef struct node {
    node_t * prev;
    node_t * next;
} node_t;


/**
 * Macro       : LIST_CONTAINER
 * Description : Get a pointer to the containing structure of a list node
 * Params      : @addr   pointer to a linked list node
 *               @type   type name of the containing structure
 *               @member name of the linked list member within the container (e.g., node)
 * Evaluates to a pointer to the containing structure, which can then be
 * dereferenced to access the structure's data elements.
 */
#define LIST_CONTAINER(addr, type, member) ( (type *) ( ((void *) addr) - offsetof(type,member) ) )

#define NODE_INIT(p_node) ( p_node->prev = NULL; p_node->next = NULL )

/**
 * Function    : destroy
 * Description :​ Destroy all nodes in the linked list by freeing the memory
 * Params      :​ @p_node a linked list data structure pointer
 * Returns     :​ Pointer to the head of the linked list (NULL)
 */
node_t * destroy( node_t *p_node );

/**
 * Function    : insert_at_beginning
 * Description :​ Add a node to the beginning of the list
 * Params      :​ @p_head pointer to the head node
 *               @p_new data to add
 * Returns     :​ Pointer to the head of the linked list
 */
node_t * insert_at_beginning( node_t *p_head, node_t *p_new );

/**
 * Function    : insert_at_end
 * Description :​ Add a node to the end of the linked list
 * Params      :​ @p_head pointer to the head node
 *               @p_new data to add
 * Returns     :​ Pointer to the head of the linked list
 */
node_t * insert_at_end( node_t *p_head, node_t *p_new );

/**
 * Function    : insert_at_position
 * Description :​ Add a node to the given position of the list
 * Params      :​ @p_base pointer to the base node
 *               @p_new data to add
 *               @index offset of where to add the data
 * Returns     :​ Pointer to the head of the linked list
 */
node_t * insert_at_position( node_t *p_base, node_t *p_new, size_t index );

/**
 * Function    : delete_from_beginning
 * Description :​ Delete the node at the beginning of the list
 * Params      :​ @p_head pointer to the head node
 * Returns     :​ Pointer to the head of the linked list (or NULL)
 */
node_t * delete_from_beginning( node_t *p_head );

/**
 * Function    : delete_from_end
 * Description :​ Delete the node at the end of the list
 * Params      :​ @p_head pointer to the head node
 * Returns     :​ Pointer to the head of the linked list (or NULL)
 */
node_t * delete_from_end( node_t *p_head );

/**
 * Function    : delete_from_position
 * Description :​ Delete the node at a given position in the list
 * Params      :​ @p_base pointer to the head node
 *               @index offset of where to remove the data
 * Returns     :​ Pointer to the head of the linked list (or NULL)
 */
node_t * delete_from_position( node_t *p_base, size_t index );

/**
 * Function    : size
 * Description :​ Return the number of nodes in the list
 * Params      :​ @p_node A node pointer in the list
 * Returns     :​ Number of nodes
 */
size_t size( node_t *p_node );


#endif /* _LINKED_LIST_H */
