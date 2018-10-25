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
}

int Controller::getNumSwitches() {
    /* Returns the number of switches */
    return nSwitches;
}


void Controller::print(){}

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

int Controller::makeFIFO(const char *pathName) {
  /* Make the FIFO */
  return mkfifo(pathName, S_IRUSR | S_IWUSR | S_IRGRP |
                          S_IWGRP | S_IROTH | S_IWOTH);
}

int Controller::openReadFIFO(int id) {
    /* Opens a FIFO for reading a switch with id. */
    makeFIFO(getFiFoName(id, 0));
    return open(getFiFoName(id, 0), O_NONBLOCK | O_RDONLY);
}

int Controller::openWriteFIFO(int id) {
    /* Opens a FIFO for writing a switch with id. */
    makeFIFO(getFiFoName(0, id));
    return open(getFiFoName(0, id), O_NONBLOCK | O_WRONLY);
}

void Controller::addFIFOs(int port, int swID) {
    /* Add FIFOs for reading and writing for a switch to list of FIFOs. */
    conns[port].rfd = openReadFIFO(swID);
    conns[port].wfd = openWriteFIFO(swID);
    // Add the connection in the connections array.
}

const char* Controller::getFiFoName(int x, int y) {
  std::string s = "fifo-" + std::to_string(x) + "-" + std::to_string(y);
  return s.c_str();
}
