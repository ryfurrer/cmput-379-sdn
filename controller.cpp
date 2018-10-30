#include "controller.h"

/*FIFO stuff*/
#include <sys/types.h>
#include <sys/stat.h>


#include <stdio.h> /* printf */
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

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


void Controller::print(){
  printf("Switch information: \n");


  //print out switch info
  for (unsigned int i = 0; i < 1; i++) {
    printf("[sw%i] port1= %i, port2= %i, port3= %i-%i\n",
            i, 0, 0, 0, 0);
  }

  printf("\n");
  printf("Packet Stats: \n");
  printf("\tReceived:\t OPEN:%i, QUERY:%i\n", 0, 0);
  printf("\tTransmitted:\t ACK:%i, ADD:%i\n", 0, 0);
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


void Controller::doIfValidPacket(FRAME packet) {
  if (packet.type == OPEN) {
    openCount++;
  } else if (packet.type == QUERY) {
    queryCount++;
  } else {
    //invalid types counters?
    printf("Unexpected packet type received\n");
  }
}


void Controller::run() {
    struct pollfd pfds[nSwitches+1];
    char buf[BUF_SIZE];

    // setup file descriptions or stdin and all connection FIFOs
    pfds[0].fd = STDIN_FILENO;
    pfds[0].events = POLLIN;
    for (int i = 1; i <= nSwitches; i++) { // note <= 
        pfds[i].fd = connections[i - 1].rfd;
        pfds[i].events = POLLIN;
    }
    

    for (;;) {
        /*
         * 1. Poll the keyboard for a user command. The user can issue one of the following commands.
         *       list: The program writes all entries in the flow table, and for each transmitted or received
         *             packet type, the program writes an aggregate count of handled packets of this
         *             type.
         *       exit: The program writes the above information and exits.
         */
        int ret = poll(pfds, PDFS_SIZE, 0);
        if (errno || ret < 0) {
            perror("ERROR: poll failure");
            exit(errno);
        }
        
        if (pfds[0].revents & POLLIN) {
            ssize_t r = read(pfds[PDFS_STDIN].fd, buf, BUF_SIZE);
            string cmd = string(buf);
            trimWhitespace(cmd);

            doIfValidCommand(cmd);
        }

        /*
         * 2. Poll the incoming FIFOs from the controller and the attached switches. The switch handles
         *    each incoming packet, as described in the Packet Types.
         */
        for (int i = 1; i <= nSwitches; i++) {
            if (pfds[i].revents & POLLIN) {
                printf("pfds[%i] has data.\n", i);
                read(pfds[i].fd, buf, BUF_SIZE);
                string cmd = string(buf);

                // take the message and parse it into a packet
                FRAME packet = rcvFrame(pfds[i].fd);
                doIfValidPacket(packet);
            }
        }
        // clear buffer
        memset(buf, 0, sizeof buf);
    }
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

void Controller::makeAllFifos(){
  for (int i = 0; i < nSwitches; i++) {
    addFIFOs(i, i); // port and swID are the same
  }
  printf("Controller fifos made. \n");
}

void Controller::addFIFOs(int port, int swID) {
    /* Add FIFOs for reading and writing for a switch to list of FIFOs. */
    conns[port].rfd = openReadFIFO(swID, 0);
    conns[port].wfd = openWriteFIFO(0, swID);
    // Add the connection in the connections array.
}