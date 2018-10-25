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

       void sendAckPacket
       void sendOpenPacket
       void sendQueryPacket
       void sendAddPacket
       void sendRelayPacket

*/

#ifndef _packet_
#define  _packet_

#include "flowTable.h"

#define MAXLINE   132
#define MAXWORD    32
#define MSG_KINDS 5


typedef enum {ACK, OPEN, QUERY, ADD, RELAY} P_TYPES;


typedef struct {

} MSG_OPEN;
//message contains the switch number,
//the numbers of its neighbouring switches (if any),
//and the range of IP addresses served by the switch
typedef struct {

} MSG_QUERY;
typedef struct {

} MSG_RELAY; //  received packet header info

typedef union {
  MSG_OPEN open;
  MSG_QUERY query;
  flow_entry fe; //add
  MSG_RELAY relay;
} MSG; // ACK has no message

typedef struct { P_TYPES type; MSG msg; } FRAME;
#endif
