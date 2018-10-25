/*
http://webdocs.cs.ualberta.ca/~c379/F18/379only/lab-messages.html
Adapted from lab code.

Messages and Frames
It is convenient to carry each message within a frame
that has extra information. In our design here, each
frame is a pair: [kind, msg] where
kind is an integer that indicates the carried message type, and
msg is a struct or union that carries the message's data.
Recall that in the ANSI standard, a struct (or union)
can be assigned to, passed to a function, and returned by a function.

#include  ...    // stdio.h, stdlib.h, unistd.h, stdarg.h, string.h,
	  	 // assert.h, sys/types.h, sys/stat.h, fcntl.h

*/

#ifndef _packet_
#define  _packet_

#include "flowTable"

#define MAXLINE   132
#define MAXWORD    32
#define MSG_KINDS 5


typedef enum {ACK, OPEN, QUERY, ADD, RELAY} P_TYPES;

typedef struct {} MSG_ACK;
typedef struct {} MSG_OPEN;
typedef struct {} MSG_QUERY;
typedef struct {} MSG_RELAY;

typedef union {
  MSG_ACK ack;
  MSG_OPEN open;
  MSG_QUERY query;
  flow_entry fe;
  MSG_RELAY relay;
} MSG;

typedef struct { KIND kind; MSG msg; } FRAME;
#endif
