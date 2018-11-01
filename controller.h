#include "connection.h"
#include "packet.h"

#define MAX_NSW 7
#define MIN_NSW 1

using namespace std;

typedef std::vector<MSG_OPEN> OpenMSGs;

class Controller {
  public:
    Controller(int num);
    int getNumSwitches();
    void makeAllReadFifos();
    void print();
    int run();

  private:
    int nSwitches;
    int openCount;
    int queryCount;
    int ackCount;
    int addCount;
    Connection conns[MAX_NSW];
    OpenMSGs openSwitches;

    bool inSwitchRange(unsigned int swID, int lowIP, int highIP);
    flow_entry makeForwardRule(unsigned int actionVal, unsigned int swID);
    flow_entry makeDropRule(unsigned int dst_lo, unsigned int dst_hi);
    flow_entry makeFlowEntry(MSG_QUERY queryMSG);
    MSG makeAddMSG(unsigned int srcIP_lo,
                    unsigned int srcIP_hi,
                    unsigned int destIP_lo,
                    unsigned int destIP_hi,
                    unsigned int actionType,
                    unsigned int actionVal,
                    unsigned int pri,
                    unsigned int pktCount);
    void doIfValidCommand(string cmd);
    void doIfValidPacket(FRAME packet);
    void checkKeyboardPoll(struct pollfd* pfd);
    void checkFIFOPoll(struct pollfd* pfds);
    void doPolling(struct pollfd* pfds);
    void setupPollingFileDescriptors(struct pollfd* pfds);
    void respondToOPENPacket(MSG_OPEN openMSG);
    void respondToQUERYPacket(MSG_QUERY queryMSG);
    int findOpenSwitch(int id);
    void addToOpenSwitches(MSG_OPEN openMSG);
};
