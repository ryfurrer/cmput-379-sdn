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

/*FIFO stuff*/
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h> /* printf */
#include <cstring> /* string compare */


#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


Switch::Switch(int id_num, const char* datafile, unsigned int IPlow, unsigned int IPhigh){
  flow_entry init_rule = {.srcIP_lo= 0,
                          .srcIP_hi= MAXIP,
                          .destIP_lo= IPlow,
                          .destIP_hi= IPhigh,
                          .actionType= FORWARD,
                          .actionVal= 3,
                          .pri= MINPRI,
                          .pktCount= 0};
  flowTable.push_back(init_rule);
  id = id_num;
  lowIP = IPlow;
  highIP = IPhigh;
}

void Switch::print(){
  printf("Flow Table: \n");

	//print out every table entry
	for (unsigned int i = 0; i < flowTable.size(); i++) {
    flow_entry fe = flowTable[i];
    const char* action = "DROP";
    if (fe.actionType) {
      action = "FORWARD";
    }

    printf("[%i] (srcIP= %i-%i, destIP= %i-%i, action= %s, pri= %i, pktCount= %i)\n",
            i, fe.srcIP_lo, fe.srcIP_hi, fe.destIP_lo,
            fe.destIP_hi, action, fe.pri, fe.pktCount);
	}

	printf("\n");
	printf("Packet Stats: \n");
	printf("\tReceived:\t ADMIT:%i, ACK:%i, ADDRULE:%i, RELAYIN:%i\n",
        0,0,0,0);
  printf("\tTransmitted:\t OPEN:%i, QUERY:%i, RELAYOUT:%i\n",
        0,0,0);
}

int Switch::makeFIFO(const char *pathName) {
    return mkfifo(pathName, S_IRUSR | S_IWUSR | S_IRGRP |
                      S_IWGRP | S_IROTH | S_IWOTH);
}

int Switch::openReadFIFO(int swID) {
    /* Opens a FIFO for reading a switch with id. */
    makeFIFO(getFiFoName(swID, id));
    return open(getFiFoName(swID, id), O_NONBLOCK | O_RDONLY);
}

int Switch::openWriteFIFO(int swID) {
    /* Opens a FIFO for writing a switch with id. */
    makeFIFO(getFiFoName(id, swID));
    return open(getFiFoName(id, swID), O_NONBLOCK | O_WRONLY);
}

void Switch::addFIFOs(int port, int swID) {
    /* Add FIFOs for reading and writing for a switch to list of FIFOs. */
    conns[port].rfd = openReadFIFO(swID);
    conns[port].wfd = openWriteFIFO(swID);
}

const char* Switch::getFiFoName(int x, int y) {
  std::string s = "fifo-" + std::to_string(x) + "-" + std::to_string(y);
  return s.c_str();
}

void Switch::setPorts(char * swID1, char * swID2) {
  /* Set up the 3 connection ports of the switch */

  // port 0 is the controller and is assumed as such;
  conns[0].swID = 0;
  addFIFOs(0, 0);


  // port 1
  if (std::strcmp(swID1, "null") == 0) { //null
    conns[1].swID = -1;
  } else if (swID1[0] == 's' && swID1[1] == 'w'){ //valid switch name
      conns[1].swID = atoi(&swID1[2]);
      addFIFOs(1, conns[1].swID);
      printf("Port 1 set\n" );

  } else { //invalid switch name
    conns[1].swID = -1;
    printf("Port 1 not set: invalid switch name \"%s\".\n", swID1 );
  }

  // port 2
  if (std::strcmp(swID1, "null") == 0) { //null
    conns[2].swID = -1;
  } else if (swID2[0] == 's' && swID2[1] == 'w'){ //valid switch name
      conns[2].swID = atoi(&swID2[2]);
      addFIFOs(2, conns[2].swID);
      printf("Port 2 set.\n" );

  } else { //invalid switch name
    conns[2].swID = -1;
    printf("Port 2 not set: invalid switch name \"%s\".\n", swID2 );
  }

}

MSG Switch::makeOpenMSG(){
  MSG msg;
  msg.open.lowIP = lowIP;
  msg.open.highIP = highIP;
  msg.open.port1 = conns[1].swID;
  msg.open.port2 = conns[2].swID;
  msg.open.myID = id;
  return msg;
}


MSG Switch::makeRelayMSG(int srcIP, int dstIP){
  MSG msg;
	msg.relay.srcIP = srcIP;
	msg.relay.dstIP = dstIP;
  return msg;
}


MSG Switch::makeQueryMSG(int srcIP, int dstIP){
  MSG msg;
  msg.query.srcIP = srcIP;
  msg.query.dstIP = dstIP;
  msg.query.port1 = conns[1].swID;
  msg.query.port2 = conns[2].swID;
  msg.query.myID = id;
  return msg;
}
