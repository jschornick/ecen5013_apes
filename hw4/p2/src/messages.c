/*
  File        : messages.c
  Description : APES Homework 4, Problem 2

  Message handling function implementations

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw4
*/

#include <messages.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char * msg_to_str(char *buffer, msg_t *msg)
{
    if( msg == NULL ) {
        return NULL;
    }

    switch( msg->header.type ) {

        case MSG_STRING:
            sprintf( buffer, "MSG_STRING(%lu, %s)", msg->header.data_len, (char *)  msg->data );
            break;

        case MSG_LED:

            switch( ((msg_led_t *) msg->data)->on_off ) {
            case LED_OFF:
                strcpy( buffer, "MSG_LED(off)" );
                break;
            case LED_ON:
                strcpy( buffer, "MSG_LED(on)" );
                break;
            default:
                strcpy( buffer, "MSG_LED(unknown)" );
                break;

            }

            break;

        default:
            sprintf( buffer, "MSG_UNKNOWN(%u)", msg->header.type );
            break;

    }

    return buffer;
}


void print_header(msg_t *msg)
{
    printf( "Header: type(%u), data_len(%lu)\n", msg->header.type, msg->header.data_len);
}


size_t msg_to_msgbuf(void *buf, msg_t *msg)
{
    memcpy( buf, &msg->header, sizeof(msg_header_t) );
    memcpy( ((uint8_t*)buf) + sizeof(msg_header_t), msg->data, msg->header.data_len );
    return sizeof(msg_header_t) +  msg->header.data_len;
}


void msgbuf_to_msg(msg_t *msg, void *buf)
{
    memcpy( &msg->header, buf, sizeof(msg_header_t) );
    msg->data = malloc( msg->header.data_len );
    memcpy( msg->data, ((uint8_t*)buf) + sizeof(msg_header_t), msg->header.data_len );
    return;
}
