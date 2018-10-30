#include "flowTable.h"
#include "connection.h"
#include "packet.h"
#include "string"

using namespace std;

#define N_PFDS 4
#define BUF_SIZE 1024

class Switch {
  public:
    Flow_table flowTable;
    Switch(int id_num, const char* datafile,
          unsigned int IPlow, unsigned int IPhigh);
    void setPorts(char * swID1, char * swID2);
    void print();
    int run();

  private:
    int id;
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
    void readLine(ifstream& trafficFileStream);
    void doIfValidCommand(string cmd) ;
    void doIfValidPacket(FRAME packet);
    void checkKeyboardPoll(struct pollfd* pfd);
    void checkFIFOPoll(struct pollfd* pfds);
    void doPolling(struct pollfd* pfds);
    void setupPollingFileDescriptors(struct pollfd* pfds);
    void openConnectionToController();

    MSG makeOpenMSG();
    MSG makeRelayMSG(int srcIP, int dstIP);
    MSG makeQueryMSG(int srcIP, int dstIP);
};
