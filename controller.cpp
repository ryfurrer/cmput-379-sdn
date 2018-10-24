#include "controller.h"
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
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

void Controller::openReadFIFO(int swID) {
    /*
        Opens a FIFO for reading for the switch with ID swID.
    */

    // Name the FIFO

    // Make the FIFO
    mkfifo(rcvName.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    // Open the FIFO

    // Add the connection in the connections array.
}

void Controller::initializeConnection(int swID) {
}

void Controller::openConnection(char swiID) {
}