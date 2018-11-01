#include "controller.h"




#include <stdio.h> /* printf */
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>

#define BUF_SIZE 1024


Controller::Controller(int num): nSwitches(num)  {
  openCount = 0;
  queryCount = 0;
  ackCount = 0;
  addCount = 0;
}

int Controller::getNumSwitches() {
    /* Returns the number of switches */
    return nSwitches;
}

int Controller::findOpenSwitch(int id) {
  for (unsigned int i = 0; i < openSwitches.size(); i++) {
    if (openSwitches[i].myID == id) {
      return i;
    }
  }
  return -1;//not found
}


void Controller::addToOpenSwitches(MSG_OPEN openMSG) {
  if (findOpenSwitch(openMSG.myID) > -1)
    openSwitches.push_back(openMSG);
}


void Controller::print(){
  /* Print out controller info */
  printf("Switch information: \n");
  //print out switch info
  for (unsigned int i = 0; i < openSwitches.size(); i++) {
    printf("[sw%i] port1= %i, port2= %i, port3= %i-%i\n",
            openSwitches[i].myID, openSwitches[i].port1,
            openSwitches[i].port2, openSwitches[i].lowIP,
            openSwitches[i].highIP);
  }

  //print packet counts
  printf("\n");
  printf("Packet Stats: \n");
  printf("\tReceived:\t OPEN:%i, QUERY:%i\n", openCount, queryCount);
  printf("\tTransmitted:\t ACK:%i, ADD:%i\n", ackCount, addCount);
}

void Controller::doIfValidCommand(string cmd) {
  /*Check string for a valid command and if exists, execute it*/

  if (cmd == "list") { /* print Controller info */
    print();
    printf("\nPlease enter 'list' or 'exit': ");

  } else if (cmd == "exit") { /* print Controller info and exit. */
    print();
    exit(0);

  } else { /* Not a valid command */
    printf("Please enter only 'list' or 'exit:'");
  }

  fflush(stdout);
  fflush(stdin);
}


void Controller::respondToOPENPacket(MSG_OPEN openMSG){
  int fd = openWriteFIFO(openMSG.myID, 0);
  conns[openMSG.myID].wfd = fd;
  sendACK(fd);
  openCount++;
  ackCount++;
}

void Controller::respondToQUERYPacket(MSG_QUERY queryMSG){
  int fd = openWriteFIFO(queryMSG.myID, 0);
  conns[queryMSG.myID].wfd = fd;
  sendACK(fd);
  queryCount++;
  addCount++;

}

void Controller::doIfValidPacket(FRAME packet) {
  if (packet.type == OPEN) {
    respondToOPENPacket(packet.msg.open);
  } else if (packet.type == QUERY) {
    queryCount++;
  } else {
    //invalid types counters?
    printf("Unexpected packet type received\n");
  }
}


void Controller::checkKeyboardPoll(struct pollfd* pfd) {
  /* 1. Poll the keyboard for a user command. */
  char buf[BUF_SIZE];
  memset((char *)&buf, ' ', sizeof(buf));
  if (pfd->revents & POLLIN) {
    read(pfd->fd, buf, BUF_SIZE);
    string cmd = string(buf);
    trimWhitespace(cmd);
    doIfValidCommand(cmd);
  }
}


void Controller::checkFIFOPoll(struct pollfd* pfds) {
  /* 2.  Poll the incoming switch fifos
    note: pfds[0] is not used as it is the keyboard*/
  for (int i = 1; i < getNumSwitches()+1; i++) {
    if (pfds[i].revents & POLLIN) {
      FRAME packet = rcvFrame(pfds[i].fd);
      doIfValidPacket(packet);

      //reprint prompt as packet type is printed
      printf("Please enter 'list' or 'exit': ");
    }
  }
}


void Controller::doPolling(struct pollfd* pfds) {
  int ret = poll(pfds, getNumSwitches()+1, 0);
  if (errno == 4 && ret < 0) { // system call interupted: ie SIGUSR1 sent
    errno = 0;
  } else if (errno && ret < 0) {
    perror("POLL ERROR: ");
    exit(errno);
  } else {
    /* 1. Poll the keyboard for a user command. */
    checkKeyboardPoll(&pfds[0]);

    /* 2.  Poll the incoming switch fifos
    and the attached switches.*/
    checkFIFOPoll(pfds);
  }
}


void Controller::setupPollingFileDescriptors(struct pollfd* pfds) {
  // setup pfd for stdin
  pfds[0].fd = STDIN_FILENO;
  pfds[0].events = POLLIN;

  //setup pfds for read fds
  for (int i = 1; i <= getNumSwitches(); i++) {
    pfds[i].fd = conns[i-1].rfd;
    pfds[i].events = POLLIN;
  }
}

int Controller::run() {
  makeAllReadFifos();

  struct pollfd pfds[getNumSwitches()+1];

  setupPollingFileDescriptors(pfds);

  printf("Please enter 'list' or 'exit': ");
  for (;;) {
  	fflush(stdout);// flush to display output
    doPolling(pfds); // poll keyboard and FIFO polling
  }

  return -1; //never reached
}

MSG Controller::makeAddMSG(unsigned int srcIP_lo,
                            unsigned int srcIP_hi,
                            unsigned int destIP_lo,
                            unsigned int destIP_hi,
                            unsigned int actionType,
                            unsigned int actionVal,
                            unsigned int pri,
                            unsigned int pktCount){
  MSG msg;
  flow_entry rule = {.srcIP_lo= srcIP_lo,
                          .srcIP_hi= srcIP_hi,
                          .destIP_lo= destIP_lo,
                          .destIP_hi= destIP_hi,
                          .actionType= actionType,
                          .actionVal= actionVal,
                          .pri= pri,
                          .pktCount= pktCount};
  msg.add = rule;
  return msg;
}

void Controller::makeAllReadFifos(){
  for (int port = 1; port <= nSwitches; port++) {
    // port and swID are the same
    conns[port-1].rfd = openReadFIFO(0, port);
  }
  printf("Controller read fifos opened. \n");
}

//int main(int argc, char *argv[]) { return 0;}
