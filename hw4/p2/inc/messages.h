/*
  File        : messages.h
  Description : APES Homework 4, Problem 2

  Message handling macros and declarations

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw4
*/


#ifndef _MESSAGES_H
#define _MESSAGES_H

#include <stdint.h>
#include <stddef.h>

// Message types
#define MSG_STRING  0
#define MSG_LED     1

// LED control values
#define LED_OFF 0
#define LED_ON  1


// Common message header
typedef struct msg_header {
    uint8_t type;     // Message type, must be one of the MSG_* macros
    size_t data_len;  // Bytes of data following the header
} msg_header_t;

// Generic message. The data pointer can be assigned to a data-specific
// structure based on the type set in the header.
typedef struct msg {
    msg_header_t header;
    void *data;
} msg_t;

// LED control message data type. This currently holds only a single value,
// but could be made more complex.
typedef struct msg_data_led {
    uint8_t on_off;
} msg_led_t;


// Function: msg_to_structure
//
// Convert a message to a printable string, using the provided buffer
// as storage for the result.
char * msg_to_str(char *buffer, msg_t *msg);


// Function: print_header
//
// Print the message header for debugging purposes
void print_header(msg_t *msg);


// Function: msg_to_msgbuf
//
// Convert from a structured message to a flat buffer for transmission.
// The header and data are both copied into the pre-allocated buffer.
size_t msg_to_msgbuf(void *buf, msg_t *msg);


// Function: msg_to_msgbuf
//
// Convert a flattened message buffer to a structured message type.
// Memory for the message data is dynamically allocated and copied
// over from the buffer, and must be manually freed before the
// message goes out of scope.
void msgbuf_to_msg(msg_t *msg, void *buf);

#endif /* _MESSAGES_H */
