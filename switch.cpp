/*

In SDN, each switch is a simple device that stores a flow table. Each entry in the table includes
• A set of packet header field values to which an incoming packet will be matched
• A set of actions to be taken when a packet matches a flow table entry
• A set of counters that are updated as packets are matched to flow table entries. These actions
might be to forward the packet to a given output switch port, or to drop the packet.
• A priority field to disambiguate table entries that may each match the header fields of an
incoming packet.

*/

#include "switch.h"
#include "parsers.h"

#include <stdio.h> /* printf */
#include <cstring> /* string compare */

#include <sstream>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <cerrno>
#include <fstream>
#include <assert.h>


using namespace std;


Switch::Switch(int id_num,
  const char * datafile, unsigned int IPlow, unsigned int IPhigh) {
  flow_entry init_rule = {
    .srcIP_lo = 0,
    .srcIP_hi = MAXIP,
    .destIP_lo = IPlow,
    .destIP_hi = IPhigh,
    .actionType = FORWARD,
    .actionVal = 3,
    .pri = MINPRI,
    .pktCount = 0
  };

  myDelay = 0;
  flowTable.push_back(init_rule);
  id = id_num;
  trafficFile = datafile;
  lowIP = IPlow;
  highIP = IPhigh;
  admitCount = 0;
  ackCount = 0;
  addCount = 0;
  openCount = 0;
  queryCount = 0;
  relayInCount = 0;
  relayOutCount = 0;
}


void Switch::printFlowTable() {
  //print out every table entry
  for (unsigned int i = 0; i < flowTable.size(); i++) {
    flow_entry fe = flowTable[i];
    const char * action = "DROP";
    if (fe.actionType) {
      action = "FORWARD";
    }

    printf("[%i] (srcIP= %i-%i, destIP= %i-%i, action= %s: %i, pri= %i, pktCount= %i)\n",
      i, fe.srcIP_lo, fe.srcIP_hi, fe.destIP_lo,
      fe.destIP_hi, action, fe.actionVal, fe.pri, fe.pktCount);
  }
}


void Switch::printPacketStats() {
  //print out packet stats
  printf("\tReceived:\t ADMIT:%i, ACK:%i, ADDRULE:%i, RELAYIN:%i\n",
    admitCount, ackCount, addCount, relayInCount);
  printf("\tTransmitted:\t OPEN:%i, QUERY:%i, RELAYOUT:%i\n",
    openCount, queryCount, relayOutCount);
}


void Switch::print() {
  /* writes all entries in the flow table, and
  for each transmitted or received
  packet type, writes an aggregate count of
  handled packets of this type. */

  printf("Flow Table: \n");
  printFlowTable();
  printf("\nPacket Stats: \n");
  printPacketStats();
}

int Switch::getFlowEntryIndex(unsigned int src, unsigned int dst) {
  /* returns flowTable rule index if IPs are in range */
  for (unsigned int index = 0; index < flowTable.size(); index++) {
    if (src >= flowTable[index].srcIP_lo && src <= flowTable[index].srcIP_hi &&
    dst >= flowTable[index].destIP_lo && dst <= flowTable[index].destIP_hi) {
      return index;
    }
  }
  return -1; // not in flowTable
}

void Switch::relayToDifferentPort(int fi, int src, int dst) {
  /*Sends packet info to port in flowTable[fi].actionVal*/
  flow_entry rule = flowTable.at(fi);
  // action val should be 1 or 2
  sendRELAY(conns[rule.actionVal].wfd, makeRelayMSG(src, dst));
  relayOutCount++;
}

void Switch::handleQuery(int src, int dst){
  /*Sends query and handles response.*/
  flow_entry fe = sendQUERY(conns[0].wfd, conns[0].rfd, makeQueryMSG(src, dst));
  queryCount++;
  if (fe.srcIP_hi){ // add rule if non null
    flowTable.push_back(fe);
    addCount++;
    if (fe.actionType == FORWARD) {
      conns[fe.actionVal].wfd = openWriteFIFO(fe.actionVal, id);
      relayToDifferentPort(flowTable.size()-1, src, dst);
    }
  }
}

void Switch::processMyTraffic(int src, int dst) {
  /*Processes the packets for this switch  from file*/
  int fi = getFlowEntryIndex(src, dst);
  admitCount++;
  if (fi >= 0) { // found rule
    flow_entry rule = flowTable.at(fi);
    if (rule.actionType == FORWARD && rule.actionVal == 3) {
      // our packet (traffic has no data, so no delivery)
    } else if (rule.actionType == FORWARD) {
      relayToDifferentPort(fi, src, dst);
    } else { // DROP (no port specified)
      // do nothing
    }
    flowTable[fi].pktCount++;

  } else { // no rule
    handleQuery(src, dst);
  }
}

void Switch::delayReading(clock_t delay) {
  // sets myDelay to the 'delay' ms in the future
  printf("Delaying for %ul ms", delay);
  myDelay = delay + std::clock();
}

void Switch::readLine(string line) {
    switch(getTrafficFileLineType(line)) {
        case T_TYPES.DELAY:
            delayPacket p = parseTrafficDelayItem(line);
            if (p.swiID == id)
                processMyTraffic(p.srcIP, p.dstIP);
        case T_TYPES.ROUTE:
            routePacket p = parseTrafficRouteItem(line);
            if (p.swiID == id)
                delayReading(p.delay);
    }
}

void Switch::readLine(ifstream& trafficFileStream) {
    /*1. Read and process a single line from the traffic file (if the EOF has
    not been reached yet). The switch ignores empty lines, comment lines,
    and lines specifying other handling switches. A packet header is
    considered admitted if the line specifies the current switch.
    */
    string line;
    // printf("Reading\n");
    if (trafficFileStream.is_open()) {
      if (getline(trafficFileStream, line)) {
        readLine(line);
      } else {
        trafficFileStream.close();
        printf("Traffic file read\n");
        printf("Please enter 'list' or 'exit': ");
      }
    }
}

void Switch::doIfValidCommand(string cmd) {
  /*Check string for a valid command and if exists, execute it*/

  if (cmd == "list") { /* print switch info */
    print();
    printf("\nPlease enter 'list' or 'exit': ");

  } else if (cmd == "exit") { /* print switch info and exit. */
    print();
    exit(0);

  } else { /* Not a valid command */
    printf("%s not valid.\n", cmd.c_str());
    printf("Please enter only 'list' or 'exit':");
  }

  fflush(stdout);
  fflush(stdin);
}


void Switch::doIfValidPacket(FRAME packet) {
  /* Only handles RELAYIN packets*/
  if (packet.type == RELAY) {
    relayInCount++;
    flowTable[0].pktCount++;
  } else {
    //invalid type counter?
    printf("Unexpected packet type received\n");
  }
}


void Switch::checkKeyboardPoll(struct pollfd* pfd) {
  /* 2. Poll the keyboard for a user command. */
  char buf[BUF_SIZE];
  memset((char *)&buf, ' ', sizeof(buf));
  if (pfd->revents & POLLIN) {
    read(pfd->fd, buf, BUF_SIZE);
    string cmd = string(buf);
    trimWhitespace(cmd);
    doIfValidCommand(cmd);
  }
}


void Switch::checkFIFOPoll(struct pollfd* pfds) {
  /* 3.  Poll the incoming FIFOs from the controller
    and the attached switches.
    note: pfds[0] is not used as it is the keyboard*/
  for (int i = 1; i < N_PFDS; i++) {
    if (pfds[i].revents & POLLIN) {
      FRAME packet = rcvFrame(pfds[i].fd);
      doIfValidPacket(packet);
      //reprint prompt as packet type is printed
      printf("Please enter 'list' or 'exit': ");
    }
  }
}


void Switch::doPolling(struct pollfd* pfds) {
  int ret = poll(pfds, N_PFDS, 0);
  if (errno == 4 && ret < 0) { // system call interupted: ie SIGUSR1 sent
    errno = 0;
  } else if (errno && ret < 0) {
    perror("POLL ERROR: ");
    exit(errno);
  } else {
    /* 2. Poll the keyboard for a user command. */
    checkKeyboardPoll(&pfds[0]);

    /* 3.  Poll the incoming FIFOs from the controller
    and the attached switches.*/
    checkFIFOPoll(pfds);
  }

}


void Switch::setupPollingFileDescriptors(struct pollfd* pfds) {
  // setup pfd for stdin
  pfds[0].fd = STDIN_FILENO;
  pfds[0].events = POLLIN;

  //setup pfds for read fds
  for (int i = 1; i < N_PFDS; i++) {
    pfds[i].fd = conns[i - 1].rfd;
    pfds[i].events = POLLIN;
  }
}


void Switch::openConnectionToController() {
  /* Sends Open packets until a ACK is recieved */
  conns[0].wfd = openWriteFIFO(0, id);
  while(!sendOPEN(conns[0].wfd, conns[0].rfd, makeOpenMSG())){}
  openCount++;
  ackCount++;
}


int Switch::run() {
  // need pollfds for every read fifo and keyboard
  struct pollfd pfds[N_PFDS];
  ifstream trafficFileStream(trafficFile);

  setupPollingFileDescriptors(pfds);
  openConnectionToController();

  for (;;) {
  	fflush(stdout);// flush to display output
    if (std::clock() > myDelay) // handles delays
        readLine(trafficFileStream);
    doPolling(pfds); // poll keyboard and FIFO polling
  }
  return 0;
}


void Switch::addFIFOs(int port, int swID) {
  /* Add FIFOs for reading a switch to list of FIFOs. */
  conns[port].rfd = openReadFIFO(id, swID);
  //conns[port].wfd = openWriteFIFO(id, swID);
}


void Switch::setPorts(char * swID1, char * swID2) {
  /* Set up the 3 connection ports of the switch */

  // port 0 is the controller and is assumed as such;
  conns[0].swID = 0;
  addFIFOs(0, 0);

  // port 1
  if (std::strcmp(swID1, "null") == 0) { //null
    conns[1].swID = -1;
  } else if (swID1[0] == 's' && swID1[1] == 'w') { //valid switch name
    conns[1].swID = atoi( &swID1[2]);
    addFIFOs(1, conns[1].swID);
    printf("Port 1 set\n");

  } else { //invalid switch name
    conns[1].swID = -1;
    printf("Port 1 not set: invalid switch name \"%s\".\n", swID1);
  }

  // port 2
  if (std::strcmp(swID2, "null") == 0) { //null
    conns[2].swID = -1;
  } else if (swID2[0] == 's' && swID2[1] == 'w') { //valid switch name
    conns[2].swID = atoi( &swID2[2]);
    addFIFOs(2, conns[2].swID);
    printf("Port 2 set.\n");

  } else { //invalid switch name
    conns[2].swID = -1;
  }

}


MSG Switch::makeOpenMSG() {
  MSG msg;
  msg.open.lowIP = lowIP;
  msg.open.highIP = highIP;
  msg.open.port1 = conns[1].swID;
  msg.open.port2 = conns[2].swID;
  msg.open.myID = id;
  return msg;
}


MSG Switch::makeRelayMSG(int srcIP, int dstIP) {
  MSG msg;
  msg.relay.srcIP = srcIP;
  msg.relay.dstIP = dstIP;
  return msg;
}


MSG Switch::makeQueryMSG(int srcIP, int dstIP) {
  MSG msg;
  msg.query.srcIP = srcIP;
  msg.query.dstIP = dstIP;
  msg.query.port1 = conns[1].swID;
  msg.query.port2 = conns[2].swID;
  msg.query.myID = id;
  return msg;
}

//int main(int argc, char *argv[]) { return 0;}
