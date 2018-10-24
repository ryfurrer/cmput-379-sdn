#include "flowTable.h"
#include "connStruct.h"

class Switch {
    public:
    Flow_table flowTable;
    Switch(int id, const char* datafile,
          unsigned int IPlow, unsigned int IPhigh);
    void setPorts(char * swID1, char * swID2);
    void makeFIFO(const char *pathName);

    private:
    Connection conns[4];
};
