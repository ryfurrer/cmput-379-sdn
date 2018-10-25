#include "flowTable.h"
#include "connStruct.h"

class Switch {
    public:
    Flow_table flowTable;
    Switch(int id_num, const char* datafile,
          unsigned int IPlow, unsigned int IPhigh);
    void setPorts(char * swID1, char * swID2);
    void makeFIFO(const char *pathName);
    const char* getFiFoName(int x, int y);
    void addFIFOs(int port, int swID);
    int openWriteFIFO(int swID);
    int openReadFIFO(int swID);
    void print();

    private:
      int id;
      Connection conns[3];
};
