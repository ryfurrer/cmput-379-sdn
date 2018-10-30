#include "flowTable.h"
#include "connection.h"
#include "packet.h"
#include "string"

class Switch {
    public:
    Flow_table flowTable;
    Switch(int id_num, const char* datafile,
          unsigned int IPlow, unsigned int IPhigh);
    void setPorts(char * swID1, char * swID2);
    int makeFIFO(const char *pathName);
    const char* getFiFoName(int x, int y);
    void addFIFOs(int port, int swID);
    int openWriteFIFO(int swID);
    int openReadFIFO(int swID);
    void print();
    int run();
    MSG makeOpenMSG();
    MSG makeRelayMSG(int srcIP, int dstIP);
    MSG makeQueryMSG(int srcIP, int dstIP);

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
};
