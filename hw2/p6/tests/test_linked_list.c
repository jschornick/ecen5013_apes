/**
 * @file test_linked_list.c
 * @brief CMocka unittests for linked list functions
 *
 * @author Jeff Schornick
 **/

#include <stdarg.h>  /* for cmocka */
#include <stddef.h>  /* for cmocka */
#include <setjmp.h>  /* for cmocka */
#include <cmocka.h>
#include <stdlib.h>
#include "linked_list.h"

// example container
typedef struct info {
    int32_t data;
    node_t node;
} info_t;

#include <stdio.h>

// print linked list when debugging tests
void print_list(node_t *p_head)
{
    node_t * p_node;
    printf("Forward:\n");
    for( p_node = p_head; p_node != NULL; p_node = p_node->next ) {
        printf( "  Data: %d\n", LIST_CONTAINER(p_node, info_t, node)->data );
    }

    printf("Reverse:\n");
    p_node = get_tail(p_head);
    for( p_node = get_tail(p_head); p_node != NULL; p_node = p_node->prev ) {
        printf( "  Data: %d\n", LIST_CONTAINER(p_node, info_t, node)->data );
    }
    printf( "Size: %zu\n\n", size(p_head));
}

void test_ll_container(void **state)
{
    info_t *p_info;
    info_t *p_container;

    p_info = malloc( sizeof(*p_info) );
    p_info->data = 42;

    p_container = LIST_CONTAINER( &p_info->node, info_t, node);
    assert_ptr_equal( p_info, p_container );

    free( p_info );

}

void test_ll_insert_at_begin(void **state)
{
    node_t *p_head = NULL;
    info_t *p_info;

    const uint items = 5;

    // isert at head, should result in a list in reverse order
    for( int i = 0; i < items; i++ ) {
        p_info = malloc( sizeof(*p_info) );
        p_info->data = i;
        p_head = insert_at_beginning( p_head, &p_info->node );
    }

    // simple backlink test
    assert_ptr_equal(p_head, p_head->next->prev);

    //print_list(p_head);

    node_t *p_node = p_head;

    assert_null( p_node->prev );

    // walk forward, verify each item was queued at the front
    for( int i = 0; i < items; i++ ) {
        assert_non_null(p_node);
        assert_int_equal( LIST_CONTAINER(p_node, info_t, node)->data, (items-1)-i );
        p_node = p_node->next;
    }
    assert_null( p_node );

    // walk backward, verify each item was queued at the front
    p_node = get_tail(p_head);
    for( int i = 0; i < items; i++ ) {
        assert_non_null(p_node);
        assert_int_equal( LIST_CONTAINER(p_node, info_t, node)->data, i );
        p_node = p_node->prev;
    }

    assert_null( p_node );


    DESTROY(p_head, info_t, node);
}

void test_ll_insert_at_end(void **state)
{
    node_t *p_head = NULL;
    info_t *p_info;

    const uint items = 5;

    // insert at end, resulting list should be in order
    for( int i = 0; i < items; i++ ) {
        p_info = malloc( sizeof(*p_info) );
        p_info->data = i;
        p_head = insert_at_end( p_head, &p_info->node );
    }
    assert_null( p_head->prev );

    node_t *p_node = p_head;

    // verify each item was queued correctly (end queuing -> should be in order)
    for( int i = 0; i < items; i++ ) {
        assert_non_null(p_node);
        assert_int_equal( LIST_CONTAINER(p_node, info_t, node)->data, i );
        p_node = p_node->next;
    }
    assert_null( p_node );

    // walk backward
    p_node = get_tail(p_head);
    for( int i = 0; i < items; i++ ) {
        assert_non_null(p_node);
        assert_int_equal( LIST_CONTAINER(p_node, info_t, node)->data, (items-1)-i );
        p_node = p_node->prev;
    }
    assert_null( p_node );

    DESTROY(p_head, info_t, node);
}

void test_ll_destroy(void **state)
{
    node_t *p_head = NULL;
    info_t *p_info = NULL;

    const uint items = 5;

    // insert at end, resulting list should be in order
    for( int i = 0; i < items; i++ ) {
        p_info = malloc( sizeof(*p_info) );
        p_info->data = i;
        p_head = insert_at_end( p_head, &p_info->node );
    }

    assert_non_null(p_head);

    DESTROY(p_head, info_t, node);
    assert_null(p_head);

    // destroy NULL is safe
    DESTROY(p_head, info_t, node);
}

void test_ll_size(void **state)
{

    node_t *p_head = NULL;
    info_t *p_info;

    const uint items = 5;

    assert_int_equal( size(p_head), 0 );

    // insert at end, resulting list should be in order
    for( int i = 0; i < items; i++ ) {
        p_info = malloc( sizeof(*p_info) );
        p_info->data = i;
        p_head = insert_at_end( p_head, &p_info->node );
    }

    assert_int_equal( size(p_head), items );

    DESTROY(p_head, info_t, node);
    assert_int_equal( size(p_head), 0 );
}

void test_ll_get_offset(void **state)
{

    node_t *p_head = NULL;
    info_t *p_info;
    node_t *p_node;

    const uint items = 5;

    // insert at 0
    for( int i = 0; i < items; i++ ) {
        p_info = malloc( sizeof(*p_info) );
        p_info->data = i;
        p_head = insert_at_end( p_head, &p_info->node );
    }
    assert_null( p_head->prev );

    for( int i = 0; i < items; i++ ) {
        p_node = get_offset(p_head, i);
        assert_int_equal( LIST_CONTAINER(p_node, info_t, node)->data, i );
    }

    // negative offsets
    node_t *p_base = get_tail(p_head);
    for( int i = 0; i < items; i++ ) {
        p_node = get_offset(p_base, -i);
        assert_int_equal( LIST_CONTAINER(p_node, info_t, node)->data, (items-1)-i );
    }

    // offsetting out of list returns null
    assert_null( get_offset(p_head, items) );
    assert_null( get_offset(get_tail(p_head), -items) );

    // nested offset
    p_node = get_offset(get_offset(p_head, 2), -1);
    assert_int_equal( LIST_CONTAINER(p_node, info_t, node)->data, 1 );

    p_node = get_offset(p_head, 0);
    assert_null( p_node->prev );

    DESTROY( p_head, info_t, node );
}

void test_ll_insert_at_position_0(void **state)
{

    node_t *p_head = NULL;
    info_t *p_info;

    const uint items = 5;

    // insert at 0
    for( int i = 0; i < items; i++ ) {
        p_info = malloc( sizeof(*p_info) );
        p_info->data = i;
        p_head = insert_at_position( p_head, &p_info->node, 0 );
    }
    assert_null( p_head->prev );
    //print_list(p_head);

    node_t *p_node = p_head;

    // each was inserted at position 0, so resulting order is reversed
    for( int i = 0; i < items; i++ ) {
        assert_non_null(p_node);
        assert_int_equal( LIST_CONTAINER(p_node, info_t, node)->data, (items-1)-i );
        p_node = p_node->next;
    }
    assert_null( p_node );

    // walk backward
    p_node = get_tail(p_head);
    for( int i = 0; i < items; i++ ) {
        assert_non_null(p_node);
        assert_int_equal( LIST_CONTAINER(p_node, info_t, node)->data, i );
        p_node = p_node->prev;
    }
    assert_null( p_node );

    DESTROY(p_head, info_t, node);


    // Test 2, position 0 from end

    p_head = NULL;
    // first setup ordered list
    for( int i = 0; i < items; i++ ) {
        p_info = malloc( sizeof(*p_info) );
        p_info->data = i;
        p_head = insert_at_end( p_head, &p_info->node );
    }
    // add to end
    p_info = malloc( sizeof(*p_info) );
    p_info->data = 99;
    p_head = insert_at_position( get_tail(p_head), &p_info->node, 0 );
    // should be next to last
    assert_int_equal( LIST_CONTAINER( get_tail(p_head)->prev, info_t, node)->data, 99 );

    DESTROY(p_head, info_t, node);
}

void test_ll_insert_at_position_positive(void **state)
{

    node_t *p_head = NULL;
    info_t *p_info = NULL;
    node_t *p_node = NULL;

    const uint items = 5;

    // ordered list
    for( int i = 0; i < items; i++ ) {
        p_info = malloc( sizeof(*p_info) );
        p_info->data = i;
        p_head = insert_at_end( p_head, &p_info->node );
    }

    p_info = malloc( sizeof(*p_info) );
    p_info->data = 99;

    // position 2 -> new list should be [0, 1, 99, 2, 3, ...]
    insert_at_position( p_head, &p_info->node, 2);
    assert_int_equal( LIST_CONTAINER( p_head->next->next, info_t, node)->data, 99 );

    p_info = malloc( sizeof(*p_info) );
    p_info->data = 0xbad;

    // too large an offset, item not inserted
    insert_at_position(p_head, &p_info->node, items+99);
    for( p_node = p_head; p_node != NULL; p_node = p_node->next ) {
        assert_int_not_equal( LIST_CONTAINER( p_node, info_t, node)->data, 0xbad );
    }

    DESTROY(p_head, info_t, node);
}

void test_ll_insert_at_position_negative(void **state)
{

    node_t *p_head = NULL;
    info_t *p_info = NULL;
    node_t *p_node = NULL;

    const uint items = 5;

    // ordered list
    for( int i = 0; i < items; i++ ) {
        p_info = malloc( sizeof(*p_info) );
        p_info->data = i;
        p_head = insert_at_end( p_head, &p_info->node );
    }

    p_info = malloc( sizeof(*p_info) );
    p_info->data = 99;

    // position -2 from end -> new list should be [0, 1, 99, 2, 3, 4]
    insert_at_position( get_tail(p_head), &p_info->node, -2);
    assert_int_equal( LIST_CONTAINER( get_tail(p_head)->prev->prev->prev, info_t, node)->data, 99 );

    p_info = malloc( sizeof(*p_info) );
    p_info->data = 0xbad;

    // too large an offset, should not insert
    insert_at_position(get_tail(p_head), &p_info->node, -99);
    for( p_node = p_head; p_node != NULL; p_node = p_node->next ) {
        assert_int_not_equal( LIST_CONTAINER( p_node, info_t, node)->data, 0xbad );
    }

    DESTROY(p_head, info_t, node);
}

void test_ll_delete_from_begin(void **state)
{
    node_t *p_head = NULL;
    info_t *p_info = NULL;

    const uint items = 5;

    // safe to delete from empty list
    assert_null( delete_from_beginning(NULL) );

    // ordered list
    for( int i = 0; i < items; i++ ) {
        p_info = malloc( sizeof(*p_info) );
        p_info->data = i;
        p_head = insert_at_end( p_head, &p_info->node );
    }

    // delete each and verify it is removed from beginning
    for( int i = 0; i < items; i++ ) {
        assert_int_equal( size(p_head), items-i );
        assert_int_equal( LIST_CONTAINER( p_head, info_t, node)->data, i );
        p_head = delete_from_beginning( p_head );
        assert_int_equal( size(p_head), (items-1)-i );
        if( i != items-1 ) {
            assert_int_equal( LIST_CONTAINER( p_head, info_t, node)->data, i+1 );
        } else {
            assert_null( p_head );
        }
    }

    DESTROY(p_head, info_t, node);

}

void test_ll_delete_from_end(void **state)
{
    node_t *p_head = NULL;
    info_t *p_info = NULL;

    const uint items = 5;

    // safe to delete from empty list
    assert_null( delete_from_end(NULL) );

    // ordered list
    for( int i = 0; i < items; i++ ) {
        p_info = malloc( sizeof(*p_info) );
        p_info->data = i;
        p_head = insert_at_end( p_head, &p_info->node );
    }

    // delete each and verify it is removed from beginning
    for( int i = 0; i < items; i++ ) {
        assert_int_equal( size(p_head), items-i );
        assert_int_equal( LIST_CONTAINER( get_tail(p_head), info_t, node)->data, (items-1)-i );

        p_head = delete_from_end( p_head );
        assert_int_equal( size(p_head), (items-1)-i );

        if( i != items-1 ) {
            assert_int_equal( LIST_CONTAINER( get_tail(p_head), info_t, node)->data, (items-1)-i-1 );
        } else {
            assert_null( p_head );
        }
    }

    DESTROY(p_head, info_t, node);

}

void test_ll_delete_from_position_0(void **state)
{
    node_t *p_head = NULL;
    info_t *p_info = NULL;
    const uint items = 5;

    // delete from empty list is safe
    assert_null( delete_from_position(NULL, 0) );

    // ordered list
    for( int i = 0; i < items; i++ ) {
        p_info = malloc( sizeof(*p_info) );
        p_info->data = i;
        p_head = insert_at_end( p_head, &p_info->node );
    }

    // Test delete offset 0 from head, list will be [1,2,3,4]
    p_head = delete_from_position( p_head, 0 );

    assert_int_equal( LIST_CONTAINER(p_head, info_t, node)->data, 1 );

    // Test delete offset 0 from tail, list will be [1,2,3]
    p_head = delete_from_position( get_tail(p_head), 0 );

    assert_int_equal( LIST_CONTAINER( get_tail(p_head), info_t, node)->data, 3 );

    // Test delete offset 0 from middle, list will be [1,3]
    p_head = delete_from_position( p_head->next, 0 );

    assert_int_equal( LIST_CONTAINER( p_head, info_t, node)->data, 1 );
    assert_int_equal( LIST_CONTAINER( p_head->next, info_t, node)->data, 3 );

}

void test_ll_delete_from_position_positive(void **state)
{
    node_t *p_head = NULL;
    info_t *p_info = NULL;
    const uint items = 5;

    // ordered list
    for( int i = 0; i < items; i++ ) {
        p_info = malloc( sizeof(*p_info) );
        p_info->data = i;
        p_head = insert_at_end( p_head, &p_info->node );
    }

    // Test delete offset 2 from head, list will be [0,1,3,4]
    p_head = delete_from_position( p_head, 2 );

    assert_int_equal( LIST_CONTAINER(p_head->next->next, info_t, node)->data, 3 );

    // too large an offset, nothing deleted
    assert_int_equal( size(p_head), items-1 );
    delete_from_position(p_head, 999);
    assert_int_equal( size(p_head), items-1 );

    DESTROY(p_head, info_t, node);
}

void test_ll_delete_from_position_negative(void **state)
{
    node_t *p_head = NULL;
    info_t *p_info = NULL;
    const uint items = 5;

    // ordered list
    for( int i = 0; i < items; i++ ) {
        p_info = malloc( sizeof(*p_info) );
        p_info->data = i;
        p_head = insert_at_end( p_head, &p_info->node );
    }

    // Test delete offset 2 from head, list will be [0,1,3,4]
    p_head = delete_from_position( get_tail(p_head), -2 );

    assert_int_equal( LIST_CONTAINER(p_head->next->next, info_t, node)->data, 3 );

    // too large an offset, nothing deleted
    assert_int_equal( size(p_head), items-1 );
    delete_from_position(get_tail(p_head), -999);
    assert_int_equal( size(p_head), items-1 );

    DESTROY(p_head, info_t, node);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_ll_container),
    cmocka_unit_test(test_ll_insert_at_begin),
    cmocka_unit_test(test_ll_insert_at_end),
    cmocka_unit_test(test_ll_destroy),
    cmocka_unit_test(test_ll_size),
    cmocka_unit_test(test_ll_get_offset),
    cmocka_unit_test(test_ll_insert_at_position_0),
    cmocka_unit_test(test_ll_insert_at_position_positive),
    cmocka_unit_test(test_ll_insert_at_position_negative),
    cmocka_unit_test(test_ll_delete_from_begin),
    cmocka_unit_test(test_ll_delete_from_end),
    cmocka_unit_test(test_ll_delete_from_position_0),
    cmocka_unit_test(test_ll_delete_from_position_positive),
    cmocka_unit_test(test_ll_delete_from_position_negative),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

