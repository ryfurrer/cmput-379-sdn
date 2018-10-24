#include "connStruct.h"

#define MAX_NSW 7

class Controller {
    public:
    Controller(int num);
    int getNumSwitches();
    void openConn(char id);
    int makeFIFO(const char *pathName);
    int openReadFIFO(int id); //returns the fd
    int openWriteFIFO(int id); //returns the fd
    void addFIFOs(int id); //returns the fd
    void initConn(int id);
    const char* getFiFoName(int x, int y);

    private:
    int nSwitches;
    Connection conns[MAX_NSW];
};
