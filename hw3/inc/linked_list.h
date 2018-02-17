/*
  File        : linked_list.h
  Description : 

  Doubly-linked list data structure library, structures and declarations.

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3

  Testing     : CMocka unit tests can be run via "make tests"
*/

#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include <stddef.h>  // offsetof
#include <stdint.h>

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

/**
 * Macro       : DESTROY
 * Description : Destroys a list and frees each allocated element
 * Params      : @p_head pointer to the head of the list
 *               @type   type name of the containing structure
 *               @member name of the linked list member within the container (e.g., node)
 */
#define DESTROY( p_head, type, member ) (                   \
        {                                                   \
            node_t *p_next;                                 \
            while(p_head) {                                 \
                p_next = p_head->next;                      \
                free(LIST_CONTAINER(p_head, type, member)); \
                p_head = p_next;                            \
            }                                               \
        }                                                   \
        )

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
 *               @p_new  data to add
 *               @offset offset describing where to add the data
 * Returns     :​ Pointer to the head of the linked list, or NULL if bad offset
 */
node_t * insert_at_position( node_t *p_base, node_t *p_new, int32_t offset );

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
 *               @offset offset describing where to remove the data
 * Returns     :​ Pointer to the head of the linked list (or NULL)
 */
node_t * delete_from_position( node_t *p_base, int32_t index );

/**
 * Function    : size
 * Description :​ Return the number of nodes in the list
 * Params      :​ @p_node A node pointer in the list
 * Returns     :​ Number of nodes
 *
 * NOTE: This function is dangerous and makes little sense as specified.
 * The onus is left on the caller to clean up the allocated container
 * memory, but is not given any indication which item was deleted from
 * the list.
 */
size_t size( node_t *p_node );

/**
 * Function    : get_head
 * Description :​ returns the node at the head of the list
 * Params      :​ @p_node pointer to a node in the list
 * Returns     :​ Pointer to the head of the linked list (or NULL)
 */
node_t * get_head( node_t *p_node );

/**
 * Function    : get_tail
 * Description :​ returns the node at the tail of the list
 * Params      :​ @p_node pointer to a node in the list
 * Returns     :​ Pointer to the head of the linked list (or NULL)
 */
node_t * get_tail( node_t *p_node );

/**
 * Function    : get_offset
 * Description :​ returns the node at the tail of the list
 * Params      :​ @p_node pointer to a node in the list
 * Returns     :​ Pointer to the head of the linked list (or NULL)
 */
node_t * get_offset( node_t *p_node, int32_t offset );

#endif /* _LINKED_LIST_H */
