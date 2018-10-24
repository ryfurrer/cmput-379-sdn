#ifndef _Connection_
#define  _Connection_

#include <string>

struct Connection {
    int swID;
    std::string rFIFO;
    std::string wFIFO;
    int rfd;
    int wfd;
};

#endif
