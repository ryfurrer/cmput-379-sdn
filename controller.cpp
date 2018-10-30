#include "controller.h"

/*FIFO stuff*/
#include <sys/types.h>
#include <sys/stat.h>


#include <stdio.h> /* printf */
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>



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