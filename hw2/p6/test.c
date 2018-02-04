// test.c

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "linked_list.h"

// just an example ?
typedef struct info {
    uint32_t data;
    node_t node;
} info_t;

int main(void)
{
    node_t *p_head = NULL;

    info_t *p_info1, *p_info2;

    p_info1 = malloc( sizeof(*p_info1) );
    p_info1->data = 42;
    p_head = insert_at_beginning( p_head, &p_info1->node );

    p_info2 = malloc( sizeof(*p_info2) );
    p_info2->data = 23;
    p_head = insert_at_beginning( p_head, &p_info2->node );

    for( node_t * p_node = p_head; p_node != NULL; p_node = p_node->next ) {
        printf( "Data: %u\n", LIST_CONTAINER(p_node, info_t, node)->data );
    }

    return 0;

}
