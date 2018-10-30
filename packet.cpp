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
 printf("%s packet received\n", convertTypeToChar(frame.type));

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
  printf("%s packet sent\n", convertTypeToChar(frame.type));
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
  printf(".%s.", cmd.c_str());
  int i = 0;
  while (cmd.at(i) == ' ')
    i++;
  int j = cmd.length() - 1;
  while (cmd.at(j) == ' ')
    j--;
  cmd = cmd.substr(i, j - i + 1);
}

void sendACK(int fd) {
  MSG msg;
  sendPacket(fd, ACK, msg);
}

bool sendOPEN(int fd, MSG msg) {
  sendPacket(fd, OPEN, msg);
  return true;
}

void sendQUERY(int fd, MSG msg) {
  sendPacket(fd, QUERY, msg);

}

void sendADD(int fd, MSG msg) {
  sendPacket(fd, ADD, msg);
}

void sendRELAY(int fd, MSG msg) {
  sendPacket(fd, RELAY, msg);
}
