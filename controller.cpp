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

void Controller::openReadFIFO(int id) {
    /* Opens a FIFO for reading a switch with id. */
    // Make the FIFO
    mkfifo(getFiFoName(id), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    // Open the FIFO

    // Add the connection in the connections array.
}

void Controller::initConn(int id) {
}

void Controller::openConn(char id) {
}
