#include "controller.h"

/*FIFO stuff*/
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>



Controller::Controller(int num): nSwitches(num)  {
    name = "fifo-0-0";
}

int Controller::getNumSwitches() {
    /* Returns the number of switches */
    return nSwitches;
}

void Controller::makeFIFO(const char *pathName) {
  /* Make the FIFO */
  int status = mkfifo(pathName, S_IRUSR | S_IWUSR | S_IRGRP |
                          S_IWGRP | S_IROTH | S_IWOTH);
}

int Controller::openReadFIFO(int id) {
    /* Opens a FIFO for reading a switch with id. */
    makeFIFO(getFiFoName(id));
    return open(getFiFoName(id), O_RDONLY);
}

int Controller::openWriteFIFO(int id) {
    /* Opens a FIFO for writing a switch with id. */
    makeFIFO(getFiFoName(id));
    return open(getFiFoName(id), O_WRONLY);
}

void Controller::addFIFOs(int id) {
    /* Add FIFOs for reading and writing for a switch to list of FIFOs. */
    openReadFIFO(id);
    openWriteFIFO(id);
    // Add the connection in the connections array.
    return open(getFiFoName(id), O_RDONLY);
}

void Controller::initConn(int id) {
}

void Controller::openConn(char id) {
}

const char* getFiFoName(int x, int y) {
  return "fifo-" + std::to_string(x) + "-" + std::to_string(y);
}
