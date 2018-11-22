#include "connection.h"
#include "packet.h"
#include "util.h"

#define MAX_NSW 7
#define MIN_NSW 1

using namespace std;

typedef std::vector<MSG_OPEN> OpenMSGs;

class Controller {
  public:
    Controller(int maxConns, int socket);
    int getNumSwitches();
    void makeAllReadFifos();
    void print();
    int run();

  private:
    int nSwitches;
    int sfd;
    int openCount;
    int queryCount;
    int ackCount;
    int addCount;
    Connection conns[MAX_NSW];

    void initializeConns();
    bool inSwitchRange(int swID, int lowIP, int highIP);
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
    void doIfValidPacket(int fd, FRAME packet);
    void checkKeyboardPoll(struct pollfd* pfd);
    void checkFIFOPoll(struct pollfd* pfds);
    void doPolling(struct pollfd* pfds);
    void setupPollingFileDescriptors(struct pollfd* pfds);
    void respondToOPENPacket(int fd, MSG_OPEN openMSG);
    void respondToQUERYPacket(MSG_QUERY queryMSG);
    int findOpenSwitchToForward(int high, int low);
    int findOpenSwitch(int id);
    int pollControllerSocket();
    void checkSwitchPoll(struct pollfd* pfds);
    void addToOpenSwitches(MSG_OPEN openMSG);
};
