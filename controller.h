#include "connStruct.h"

#define MAX_NSW 7

class Controller {
    public:
    Controller(int num, int max);
    int getNumSwitches();
    void openConn(char id);
    int openReadFIFO(int id); //returns the fd
    void initConn(int id);
    const char* getFiFoName(int x, int y);

    private:
    std::string name;
    int nSwitches;
    Connection conns[MAX_NSW];
};
