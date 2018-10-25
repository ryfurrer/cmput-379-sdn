#include "connStruct.h"
#include "packet.h"

#define MAX_NSW 7

class Controller {
    public:
    Controller(int num);
    int getNumSwitches();
    int makeFIFO(const char *pathName);
    int openReadFIFO(int id); //returns the fd
    int openWriteFIFO(int id); //returns the fd
    void addFIFOs(int port, int swID);
    const char* getFiFoName(int x, int y);
    void print();
    void makeAllFifos();
    MSG makeAddMSG(unsigned int srcIP_lo,
                    unsigned int srcIP_hi,
                    unsigned int destIP_lo,
                    unsigned int destIP_hi,
                    unsigned int actionType,
                    unsigned int actionVal,
                    unsigned int pri,
                    unsigned int pktCount);

    private:
    int nSwitches;
    Connection conns[MAX_NSW];
};
