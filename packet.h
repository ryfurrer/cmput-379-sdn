/*
http://webdocs.cs.ualberta.ca/~c379/F18/379only/lab-messages.html
Adapted from lab code.

Messages and Frames

*/

#ifndef _packet_
#define  _packet_

#include "flowTable.h"

#define MAXLINE   132
#define MAXWORD    32
#define MSG_KINDS 5


typedef enum {ACK, OPEN, QUERY, ADD, RELAY} P_TYPES;


typedef struct {
	int lowIP;
	int highIP;
	int port1;
	int port2;
	int myID;
} MSG_OPEN;

typedef struct {
	int srcIP;
	int dstIP;
	int port1;
	int port2;
	int myID;
} MSG_QUERY;

typedef struct {
  int srcIP;
  int dstIP;
} MSG_RELAY; //  received packet header info

typedef union {
  MSG_OPEN open;
  MSG_QUERY query;
  flow_entry add; //add
  MSG_RELAY relay;
} MSG; // ACK has no message

typedef struct { P_TYPES type; MSG msg; } FRAME;
#endif
