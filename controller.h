#include "connStruct.h"

#define MAX_NSW 7

class Controller {
    public:
    Controller(int num);
    int getNumSwitches();
    int makeFIFO(const char *pathName);
    int openReadFIFO(int id); //returns the fd
    int openWriteFIFO(int id); //returns the fd
    void addFIFOs(int port, int swID);
    const char* getFiFoName(int x, int y);
    void print();
    void makeAllFifos();

    private:
    int nSwitches;
    Connection conns[MAX_NSW];
};
