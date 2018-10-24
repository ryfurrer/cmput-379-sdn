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

//#include <stdio.h>
//#include <cstdlib>
//#include <iostream>
//#include <signal.h>
//#include <stdlib.h>
//#include <string>
//#include <sstream>
//#include <iterator>
//
//#include <unistd.h>
//
//#include <sys/resource.h>


Switch::Switch(int id, const char* datafile, unsigned int IPlow, unsigned int IPhigh){
  flow_entry init_rule = {.srcIP_lo= 0,
                          .srcIP_hi= MAXIP,
                          .destIP_lo= IPlow,
                          .destIP_hi= IPhigh,
                          .actionType= DELIVER,
                          .actionVal= 3,
                          .pri= MINPRI,
                          .pktCount= 0};
  flowTable.push_back(init_rule);
}

void Switch::makeFIFO(const char *pathName) {
    mkfifo(pathName, S_IRUSR | S_IWUSR | S_IRGRP |
                      S_IWGRP | S_IROTH | S_IWOTH);
}

void Switch::setPorts(char * swID1, char * swID2) {
  //conns[0];
}
