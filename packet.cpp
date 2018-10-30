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

//poll
//A negative value should immediately
//be followed by a check of errno, since it signifies an error.



FRAME rcvFrame(int fd)
{// http://webdocs.cs.ualberta.ca/~c379/F18/379only/lab-messages.html
 FRAME  frame;

 assert(fd >= 0);
 memset((char *)&frame, 0, sizeof(frame));
 read(fd, (char *)&frame, sizeof(frame));

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
}

void trimWhitespace(string & cmd) {
  //trim whitespace
  printf(".%s.", cmd);
  int i = 0;
  while (cmd.at(i) == ' ')
    i++;
  int j = cmd.length() - 1;
  while (cmd.at(j) == ' ')
    j--;
  cmd = cmd.substr(i, j - i + 1);
}
