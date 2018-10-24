#include "connStruct.h"

#define MAX_NSW 7

class Controller {
    public:
    Controller(int num, int max);
    int getNumSwitches();
    void openConn(char id);
    void openReadFIFO(int id);
    void initConn(int id);
    std::String getFiFoName(int id);

    private:
    std::string name;
    int nSwitches;
    Connection conns[MAX_NSW];
};
