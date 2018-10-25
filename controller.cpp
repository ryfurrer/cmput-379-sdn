#include "controller.h"

/*FIFO stuff*/
#include <sys/types.h>
#include <sys/stat.h>

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

int Controller::makeFIFO(const char *pathName) {
  /* Make the FIFO */
  int status = mkfifo(pathName, S_IRUSR | S_IWUSR | S_IRGRP |
                          S_IWGRP | S_IROTH | S_IWOTH);
  return status;
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

void Controller::addFIFOs(int id) {
    /* Add FIFOs for reading and writing for a switch to list of FIFOs. */
    openReadFIFO(id);
    openWriteFIFO(id);
    // Add the connection in the connections array.
}

const char* Controller::getFiFoName(int x, int y) {
  std::string s = "fifo-" + std::to_string(x) + "-" + std::to_string(y);
  return s.c_str();
}
