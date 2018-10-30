/*
http://webdocs.cs.ualberta.ca/~c379/F18/379only/lab-messages.html
Adapted from lab code.

Messages and Frames

*/

#ifndef _packet_
#define  _packet_

#include "flowTable.h"
#include <string>

#define MAXLINE   132
#define MAXWORD    32
#define MSG_KINDS 5

using namespace std;


typedef enum {ACK, OPEN, QUERY, ADD, RELAY} P_TYPES;

typedef struct {
	int srcIP;
	int dstIP;
} IP_LOCATIONS;

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

FRAME rcvFrame(int fd);
void sendPacket(int fd, P_TYPES type, MSG msg);
void sendACK(int fd);
void sendOPEN(int fd, MSG msg);
flow_entry sendQUERY(int fd, MSG msg);
void sendADD(int fd, MSG msg);
void sendRELAY(int fd, MSG msg);

void trimWhitespace(string & cmd);
#endif
