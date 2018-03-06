/*
  File        : messages.h
  Description : APES Homework 4, Problem 2

  Message handling

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
  https://github.com/jschornick/ecen5013_apes/hw3
*/


#ifndef _MESSAGES_H
#define _MESSAGES_H

#include <stdint.h>
#include <stddef.h>

#define MSG_STRING  0
#define MSG_LED     1

#define LED_OFF 0
#define LED_ON  1

// message header
typedef struct msg_header {
    uint8_t type;
    size_t data_len;  // bytes of data following header
} msg_header_t;

// generic message
typedef struct msg {
    msg_header_t header;
    void *data;
} msg_t;


typedef struct msg_data_led {
    uint8_t on_off;
} msg_led_t;

char * msg_to_str(char *buffer, msg_t *msg);
void print_header(msg_t *msg);

#endif /* _MESSAGES_H */
