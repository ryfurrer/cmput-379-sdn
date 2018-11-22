#include "packet.h"
#include <assert.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;




FRAME rcvFrame(int fd) {
  // receives a packet and prints its type
  // http://webdocs.cs.ualberta.ca/~c379/F18/379only/lab-messages.html
 FRAME  frame;

 assert(fd >= 0);
 memset((char *)&frame, 0, sizeof(frame));
 int val = read(fd, (char *)&frame, sizeof(frame));
 if (!val) {
   frame.type = CLOSE;
   return frame;
 }
 printf("\n%s packet received from %i (fd: %i)\n", convertTypeToChar(frame.type),
        frame.senderID, fd);

 return frame;
}

void sendPacket(int fd, int sendID, int rcvID, P_TYPES type, MSG msg){
  // sends a packet and prints its type
  FRAME frame;
  memset((char *)&frame, 0, sizeof(frame));
  frame.type = type;
  frame.senderID = sendID;
  if (type != ACK) {
    frame.msg = msg;
  }

  write(fd, (char *)&frame, sizeof(frame));
  printf("\n%s packet sent to %i\n",
         convertTypeToChar(frame.type), rcvID);
}

const char* convertTypeToChar(int type){
  if (type == ACK) {
    return "ACK";
  } else if (type == OPEN) {
    return "OPEN";
  } else if (type == QUERY) {
    return "QUERY";
  } else if (type == ADD) {
    return "ADD";
  } else if (type == RELAY) {
    return "RELAY";
  } else {
    return "UNKNOWN";
  }
}

void sendACK(int fd, int senderID, int rcvID) {
  MSG msg;
  sendPacket(fd, senderID, rcvID, ACK, msg);
}

bool sendOPEN(int wfd, int rfd, int senderID, int rcvID, MSG msg) {
  /* send open packet and poll for acknowledgement */
  sendPacket(wfd, senderID, rcvID, OPEN, msg);

  struct pollfd pfd[1];
  pfd[0].fd = rfd;
	pfd[0].events = POLLIN;

  if(poll(pfd, 1, 1000) < 0) {
    perror("OPEN PACKET POLL ERROR:");
    exit(-1);
  };

  if ((pfd[0].revents & POLLIN) == POLLIN) {
		FRAME packet = rcvFrame(rfd);
		if (packet.type == ACK) {
			return true; // ack received
		}
  }
  return false; // ack not recieved
}

flow_entry sendQUERY(int wfd, int rfd, int senderID, int rcvID, MSG msg) {
  /* Send query and get add message back */
  sendPacket(wfd, senderID, rcvID, QUERY, msg);
  struct pollfd pfd[1];
  pfd[0].fd = rfd;
	pfd[0].events = POLLIN;

  if(poll(pfd, 1, 2000) < 0) {
    perror("OPEN PACKET POLL ERROR:");
    exit(-1);
  };

  if ((pfd[0].revents & POLLIN) == POLLIN) {
		FRAME packet = rcvFrame(rfd);
		if (packet.type == ADD) {
			return packet.msg.add; // add received
		}
  }
  return flow_entry(); // ack not recieved

}

void sendADD(int fd, int senderID, int rcvID, MSG msg) {
  sendPacket(fd, senderID, rcvID, ADD, msg);
}

void sendRELAY(int fd, int senderID, int rcvID, MSG msg) {
  sendPacket(fd, senderID, rcvID, RELAY, msg);
}
