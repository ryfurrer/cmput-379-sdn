#include "flowTable.h"
#include "connection.h"
#include "packet.h"
#include "util.h"
#include "parsers.h"
#include "string"

using namespace std;

#define N_PFDS 4
#define BUF_SIZE 1024

class Switch {
  public:
    Switch(int id_num, const char* datafile,
          unsigned int IPlow, unsigned int IPhigh, int socketFD);
    void setPorts(char * swID1, char * swID2);
    void print();
    int run();

  private:
    Flow_table flowTable;
    clock_t myDelay;
    bool delayed = true;
    int id;
    int socket;
    uint16_t port;
    int lowIP;
    int highIP;
    int admitCount;
    int ackCount;
    int addCount;
    int relayOutCount;
    int openCount;
    int queryCount;
    int relayInCount;
    Connection conns[3];
    std::string trafficFile;

    void printFlowTable();
    void printPacketStats();
    void addFIFOs(int port, int swID);
    void readLine(string line);
    void delayReading(clock_t delay);
    void readLine(ifstream& trafficFileStream);
    void doIfValidCommand(string cmd) ;
    void doIfValidPacket(FRAME packet);
    void checkKeyboardPoll(struct pollfd* pfd);
    void checkFIFOPoll(struct pollfd* pfds);
    void doPolling(struct pollfd* pfds);
    void setupPollingFileDescriptors(struct pollfd* pfds);
    void openConnectionToController();
    int getFlowEntryIndex(unsigned int src, unsigned int dst);
    void relayToDifferentPort(int fi, int src, int dst);
    void processMyTraffic(int src, int dst);

    void handleQuery(int src, int dst);
    MSG makeOpenMSG();
    MSG makeRelayMSG(int srcIP, int dstIP);
    MSG makeQueryMSG(int srcIP, int dstIP);
};
