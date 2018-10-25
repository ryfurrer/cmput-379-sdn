#include "flowTable.h"
#include "connStruct.h"
#include "packet.h"

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
    MSG makeOpenMSG();
    MSG makeRelayMSG(int srcIP, int dstIP);
    MSG makeQueryMSG(int srcIP, int dstIP);

    private:
      int id;
      int lowIP;
      int highIP;
      Connection conns[3];
};
