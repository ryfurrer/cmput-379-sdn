/*
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

#define MAXLINE   132
#define MAXWORD    32

#define NF 3		 // number of fields in each message

#define MSG_KINDS 5
typedef enum { STR, INT, FLOAT, DONE, ACK } KIND;	  // Message kinds
char KINDNAME[][MAXWORD]= { "STR", "INT", "FLOAT", "DONE", "ACK" };

typedef struct { char  d[NF][MAXLINE]; } MSG_STR;
typedef struct { int   d[NF]; }          MSG_INT;
typedef struct { float d[NF]; }          MSG_FLOAT;

typedef union { MSG_STR  mStr; MSG_INT mInt; MSG_FLOAT mFloat; } MSG;

typedef struct { KIND kind; MSG msg; } FRAME;

*/

#ifndef _packet_
#define  _packet_
typedef enum {ACK, OPEN, QUERY, ADD, RELAY} P_TYPES;
#endif
