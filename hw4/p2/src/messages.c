/*
  File        : messages.c
  Description : APES Homework 4, Problem 2

  Message handling

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
  https://github.com/jschornick/ecen5013_apes/hw3
*/

#include <messages.h>
#include <stdio.h>
#include <string.h>

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
