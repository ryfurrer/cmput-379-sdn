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
#include <poll.h>
#include <sys/signalfd.h>

#include <stdio.h> /* printf */
#include <cstring> /* string compare */


#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <cerrno>



#include <assert.h>

/*FIFO stuff*/
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <fcntl.h>
#include <regex>
#include <fstream>
#include <unistd.h>
#include <poll.h>

using namespace std;



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
  /* writes all entries in the flow table, and
  for each transmitted or received
  packet type, writes an aggregate count of
  handled packets of this type. */

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

int Switch::run(){
  // need pollfds for every read fifo, keyboard and
  int n_pfds = 4;
  struct pollfd pfds[n_pfds];

  char buf[1024];
  string line;
  ifstream trafficFileStream(trafficFile);

  // setup pfd for stdin
  pfds[0].fd = STDIN_FILENO;
  pfds[0].events = POLLIN;

  //setup pfds for read fds
  for (int i = 1; i < n_pfds; i++) {
    pfds[i].fd = conns[i-1].rfd;
    pfds[i].events = POLLIN;
  }


  //send open packet to controller
  sendPacket(conns[1].wfd, OPEN, makeOpenMSG());  //move to new funct - wait for ACK

  printf("Please enter 'list' or 'exit': ");
  for(;;) {
    /*1. Read and process a single line from the traffic line (ifthe EOF has
    not been reached yet). The switch ignores empty lines, comment lines,
    and lines specifying other handling switches. A packet header is
    considered admitted if the line specifies the current switch.
    */
    if (trafficFileStream.is_open()) {
      if (getline(trafficFileStream, line)) {
        // parseTrafficFileLine(line);
      } else {
        trafficFileStream.close();
        printf("Traffic file read\n");
      }
    }


     poll(pfds, n_pfds, 0);
     /* 2. Poll the keyboard for a user command. */
     if (pfds[0].revents & POLLIN) {
       read(pfds[0].fd, buf, 1024);
       string cmd = string(buf);


       //trim whitespace
       int i=0;
       while(cmd.at(i)==' ')
         i++;
       int j=cmd.length()-1;
       while(cmd.at(j)==' ')
         j--;
       cmd=cmd.substr(i,j-i+1);

       /* list: The program writes all entries in the flow table, and
       for each transmitted or received
       packet type, the program writes an aggregate count of
       handled packets of this type. */
       if (cmd == "list") {
          print();
          printf("\nPlease enter 'list' or 'exit': ");
       /* exit: The program writes the above information and exits. */
       } else if (cmd == "exit") {
          print();
          exit(0);
       } else {
          printf("Please enter only 'list' or 'exit:'");
       }
     }
     // printf("Please enter 'list' or 'exit': ");
     // if (i == 1) return 0;
  }
  return 0;
}

int Switch::makeFIFO(const char *pathName) {
    int status = mkfifo(pathName, S_IRUSR | S_IWUSR | S_IRGRP |
                      S_IWGRP | S_IROTH | S_IWOTH);
    if (errno || status == -1) {
      printf("ERROR: error creating FIFO connection\n");
      exit(-1);
    }
    return status;
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
