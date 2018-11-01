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

//poll
//A negative value should immediately
//be followed by a check of errno, since it signifies an error.



FRAME rcvFrame(int fd)
{// http://webdocs.cs.ualberta.ca/~c379/F18/379only/lab-messages.html
 FRAME  frame;

 assert(fd >= 0);
 memset((char *)&frame, 0, sizeof(frame));
 read(fd, (char *)&frame, sizeof(frame));
 printf("\n%s packet received\n", convertTypeToChar(frame.type));

 return frame;
}

void sendPacket(int fd, P_TYPES type, MSG msg){
  FRAME frame;
  memset((char *)&frame, 0, sizeof(frame));
	frame.type = type;
  if (type != ACK) {
    frame.msg = msg;
  }

  write(fd, (char *)&frame, sizeof(frame));
  printf("\n%s packet sent\n", convertTypeToChar(frame.type));
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

void trimWhitespace(string & cmd) {
  //trim whitespace
  unsigned int i = 0;
  while (i < cmd.length() && !std::isalpha(cmd.at(i)))
    i++;
  unsigned int j = cmd.length() - 1;
  while (j > i && !std::isalpha(cmd.at(j)))
    j--;
  cmd = cmd.substr(i, j - i + 1);
}

void sendACK(int fd) {
  MSG msg;
  sendPacket(fd, ACK, msg);
}

bool sendOPEN(int wfd, int rfd, MSG msg) {
  /* send open packet and poll for acknowledgement */
  sendPacket(wfd, OPEN, msg);

  struct pollfd pfd[1];
  pfd[0].fd = rfd;
	pfd[0].events = POLLIN;
  poll(pfd, 1, 1000); //block for one seconds
  if ((pfd[0].revents & POLLIN) == POLLIN) {
		FRAME packet = rcvFrame(rfd);
		if (packet.type == ACK) {
			return true; // ack received
		}
  }
  return false; // ack not recieved
}

flow_entry sendQUERY(int wfd, int rfd, MSG msg) {
  /* Send query and get add message back */
  sendPacket(wfd, QUERY, msg);
  struct pollfd pfd[1];
  pfd[0].fd = rfd;
	pfd[0].events = POLLIN;
  poll(pfd, 1, 1000); //block for one seconds
  if ((pfd[0].revents & POLLIN) == POLLIN) {
		FRAME packet = rcvFrame(rfd);
		if (packet.type == ADD) {
			return packet.msg.add; // add received
		}
  }
  return flow_entry(); // ack not recieved

}

void sendADD(int fd, MSG msg) {
  sendPacket(fd, ADD, msg);
}

void sendRELAY(int fd, MSG msg) {
  sendPacket(fd, RELAY, msg);
}
