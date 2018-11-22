#include "controller.h"




#include <stdio.h> /* printf */
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 1024

int Controller::pollControllerSocket() {
  int new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
    struct pollfd pollSocket[1];
		pollSocket[0].fd = sfd;
		pollSocket[0].events = POLLIN;
		poll(pollSocket, 1, 0);
		if ((pollSocket[0].revents&POLLIN) == POLLIN) {
      if ((new_socket = accept(sfd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
      }

      printf("\nSocket connection accepted\n");
      for (int i = 0; i < getNumSwitches(); i++) {
        if (conns[i].rfd == -1){
          conns[i].swID = -1;
          conns[i].rfd = new_socket;
          conns[i].wfd = new_socket;
          break;
        }
      }

    }
  return EXIT_SUCCESS;
}


Controller::Controller(int maxConns, int socket): nSwitches(maxConns),
                                                        sfd(socket) {
  openCount = 0;
  queryCount = 0;
  ackCount = 0;
  addCount = 0;
  initializeConns();
}

int Controller::getNumSwitches() {
    /* Returns the number of switches */
    return nSwitches;
}

void Controller::initializeConns() {
  for (int i = 0; i < getNumSwitches(); i++) {
    conns[i].swID = -1;
    conns[i].rfd = -1;
    conns[i].wfd = -1;
    conns[i].lowIP = -1;
    conns[i].highIP = -1;
    conns[i].port1 = -1;
    conns[i].port2 = -1;
  }
}

int Controller::findOpenSwitch(int id) {
  /* checks if this switch connection index has sent an open packet to the Controller
  before. Returns its index or -1 if not found */
  for (int i = 0; i < getNumSwitches(); i++) {
    if (conns[i].swID == id) {
      return i;
    }
  }
  return -1;//not found
}

int Controller::findOpenSwitchToForward(int high, int low) {
  /* checks the cpntroller knows of a valid switch for a packetand
  returns its index */
  for (int i = 0; i < getNumSwitches(); i++) {
    if (conns[i].swID != -1 &&
      inSwitchRange(i, low, high)) {
        //low and high are the same value
      return i; // returns best switch index
    }
  }
  return -1;//not found
}


void Controller::addToOpenSwitches(MSG_OPEN openMSG) {
  /*adds new switchs' info to list when an open packet is sent*/
  if (findOpenSwitch(openMSG.myID) < 0) {
    printf("Adding switch %i\n", openMSG.myID);
  }
}


void Controller::print(){
  /* Print out controller info */
  printf("Switch information: \n");
  //print out switch info
  for (int i = 0; i < getNumSwitches(); i++) {
    if (conns[i].swID > 0) {
      printf("[sw%i] port1= %i, port2= %i, port3= %i-%i\n",
            conns[i].swID, conns[i].port1,
            conns[i].port2, conns[i].lowIP,
            conns[i].highIP);
    }
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
    close(sfd);
    exit(0);

  } else { /* Not a valid command */
    printf("%s not valid.\n", cmd.c_str());
    printf("Please enter only 'list' or 'exit':");
  }

  fflush(stdout);
  fflush(stdin);
}


void Controller::respondToOPENPacket(int fd, MSG_OPEN openMSG){
  int i = 0;
  for (i = 0; i < getNumSwitches(); i++) {
    if (conns[i].rfd == fd) {
      conns[i].swID = openMSG.myID;
      conns[i].lowIP = openMSG.lowIP;
      conns[i].highIP = openMSG.highIP;
      conns[i].port1 = openMSG.port1;
      conns[i].port2 = openMSG.port2;
      break;
    }
  }
  sendACK(fd, 0, openMSG.myID);
  openCount++;
  ackCount++;
}


/*checks if a certian switch in connection contains IPs between
 lowIP and highIP*/
bool Controller::inSwitchRange(int swID, int lowIP, int highIP) {
  if (swID >= 0 && swID < getNumSwitches() &&
      conns[swID].lowIP <= lowIP &&
      conns[swID].highIP >= highIP) {
    return true;
  }
  return false;
}

flow_entry Controller::makeForwardRule(unsigned int actionVal, unsigned int swID){
  flow_entry new_rule = {
    .srcIP_lo = 0,
    .srcIP_hi = MAXIP,
    .destIP_lo = (unsigned int) conns[swID].lowIP,
    .destIP_hi = (unsigned int) conns[swID].highIP,
    .actionType = FORWARD,
    .actionVal = actionVal,
    .pri = MINPRI,
    .pktCount = 1
  };
  return new_rule;
}

flow_entry Controller::makeDropRule(unsigned int dst_lo, unsigned int dst_hi){
  flow_entry new_rule = {
    .srcIP_lo = 0,
    .srcIP_hi = MAXIP,
    .destIP_lo = dst_lo,
    .destIP_hi = dst_hi,
    .actionType = DROP,
    .actionVal = DROP,
    .pri = MINPRI,
    .pktCount = 1
  };
  return new_rule;
}

flow_entry Controller::makeFlowEntry(MSG_QUERY queryMSG) {
  /* makes a flow entry for a add packet */
  int destPort = findOpenSwitchToForward(queryMSG.dstIP, queryMSG.dstIP);
  //nowhere to forward
  if (destPort == -1)
    return makeDropRule(queryMSG.dstIP, queryMSG.dstIP);

  //port 1 is in the correct direction
  if (conns[destPort].swID <= queryMSG.port1)
    return makeForwardRule(queryMSG.port1, destPort);
  //port 2 is the correct destination
  return makeForwardRule(queryMSG.port2, destPort);

}

void Controller::respondToQUERYPacket(MSG_QUERY queryMSG){
  /*Responds to open packets within its range of switches*/
  if (queryMSG.myID <= getNumSwitches()) {
    int index = findOpenSwitch(queryMSG.myID);
    if (index == -1) {
      perror("Cannot get query from switch before opend");
      exit(EXIT_FAILURE);
    }

    MSG msg;
    msg.add = makeFlowEntry(queryMSG);
    sendADD(conns[index].wfd, 0, queryMSG.myID, msg);
    queryCount++;
    addCount++;
  }
}

void Controller::doIfValidPacket(int fd, FRAME packet) {
  if (packet.type == OPEN) {
    respondToOPENPacket(fd, packet.msg.open);
  } else if (packet.type == QUERY) {
    respondToQUERYPacket(packet.msg.query);
  } else {
    //invalid types counters?
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


void Controller::checkSwitchPoll(struct pollfd* pfds) {
  /* 2.  Poll the incoming switch */
  for (int i = 1; i < getNumSwitches()+1; i++) {
    if (pfds[i].revents & POLLIN) {
      char buffer[32];
      if (recv(pfds[i].fd, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0) { //MSG_PEEK
        //http://www.stefan.buettcher.org/cs/conn_closed.html
        // if recv returns zero, that means the connection has been closed:
        // kill the child process
        printf("\n Note: Switchy %i be closed\n", conns[i-1].swID);
        close(pfds[i].fd);
        conns[i-1].rfd = -1;
        conns[i-1].wfd = -1;
        break;
      }
      FRAME packet = rcvFrame(pfds[i].fd);
      doIfValidPacket(pfds[i].fd, packet);
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
    pollControllerSocket();
    /* 2.  Poll the incoming switch fifos
    and the attached switches.*/
    checkSwitchPoll(pfds);
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
    setupPollingFileDescriptors(pfds);
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
  //Not used for a3
  for (int i = 0; i < getNumSwitches(); i++) {
    conns[i].rfd = -1;
  }
}

//int main(int argc, char *argv[]) { return 0;}
