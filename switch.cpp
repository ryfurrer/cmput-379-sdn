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

#include <stdio.h> /* printf */
#include <cstring> /* string compare */


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

  flowTable.push_back(init_rule);
  id = id_num;
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


void Switch::readLine(ifstream& trafficFileStream) {
    /*1. Read and process a single line from the traffic file (if the EOF has
    not been reached yet). The switch ignores empty lines, comment lines,
    and lines specifying other handling switches. A packet header is
    considered admitted if the line specifies the current switch.
    */
    string line;
    if (trafficFileStream.is_open()) {
      if (getline(trafficFileStream, line)) {
        // parseTrafficFileLine(line);
      } else {
        trafficFileStream.close();
        printf("Traffic file read\n");
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
    printf("Please enter only 'list' or 'exit:'");
  }

  fflush(stdout);
  fflush(stdin);
}


void Switch::doIfValidPacket(FRAME packet) {
  if (packet.type == ACK) {
    ackCount++;
  } else if (packet.type == ADD) {
    addCount++;
  } else if (packet.type == RELAY) {
    relayInCount++;
    admitCount++;
  } else {
    //invalid type counter?
    printf("Unexpected packet type received\n");
  }
}


void Switch::checkKeyboardPoll(struct pollfd* pfd) {
  /* 2. Poll the keyboard for a user command. */
  char buf[BUF_SIZE];
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
    }
  }
}


void Switch::doPolling(struct pollfd* pfds) {
  poll(pfds, N_PFDS, 0);
  /* 2. Poll the keyboard for a user command. */
  checkKeyboardPoll(&pfds[0]);

  /* 3.  Poll the incoming FIFOs from the controller
  and the attached switches.*/
  checkFIFOPoll(pfds);
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
    sendPacket(conns[1].wfd, OPEN, makeOpenMSG());
    //TODO: wait for ACK
}


int Switch::run() {
  // need pollfds for every read fifo and keyboard
  struct pollfd pfds[N_PFDS];
  ifstream trafficFileStream(trafficFile);

  setupPollingFileDescriptors(pfds);
  openConnectionToController();

  printf("Please enter 'list' or 'exit': ");
  for (;;) {
    readLine(trafficFileStream); // done only if EOF not reached
    doPolling(pfds); // poll keyboard and FIFO polling
  }
  return 0;
}


void Switch::addFIFOs(int port, int swID) {
  /* Add FIFOs for reading and writing for a switch to list of FIFOs. */
  conns[port].rfd = openReadFIFO(id, swID);
  conns[port].wfd = openWriteFIFO(swID, id);
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
  if (std::strcmp(swID1, "null") == 0) { //null
    conns[2].swID = -1;
  } else if (swID2[0] == 's' && swID2[1] == 'w') { //valid switch name
    conns[2].swID = atoi( &swID2[2]);
    addFIFOs(2, conns[2].swID);
    printf("Port 2 set.\n");

  } else { //invalid switch name
    conns[2].swID = -1;
    printf("Port 2 not set: invalid switch name \"%s\".\n", swID2);
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
