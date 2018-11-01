#ifndef _Connection_
#define  _Connection_

/*FIFO stuff*/
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <sys/signalfd.h>
#include <string>

struct Connection {
    int swID;
    int rfd;
    int wfd;
};

std::string getFiFoName(int x, int y);
int makeFIFO(const char * pathName);
int openReadFIFO(int swID1, int swID2);
int openWriteFIFO(int swID1, int swID2);

#endif
