#include <string.h>

struct Connection {
    int swID;
    std::string rFIFO;
    std::string wFIFO;
    int rfd;
    int wfd;
};
