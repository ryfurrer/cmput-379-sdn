#include "connection.h"
#include "packet.h"

#define MAX_NSW 7
#define MIN_NSW 1

using namespace std;

class Controller {
  public:
    Controller(int num);
    int getNumSwitches();
    void print();
    void run();

    MSG makeAddMSG(unsigned int srcIP_lo,
                    unsigned int srcIP_hi,
                    unsigned int destIP_lo,
                    unsigned int destIP_hi,
                    unsigned int actionType,
                    unsigned int actionVal,
                    unsigned int pri,
                    unsigned int pktCount);
    void addFIFOs(int port, int swID);
    void makeAllFifos();
    void doIfValidCommand(string cmd);
    void doIfValidPacket(FRAME packet);

  private:
    int nSwitches;
    int openCount;
    int queryCount;
    int ackCount;
    int addCount;
    Connection conns[MAX_NSW];
};
