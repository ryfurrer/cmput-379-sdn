#include <string.h>

#define MAX_NSW 7

struct Connection {
    int swID;
    std::string rFIFO;
    std::string wFIFO;
    int rfd;
    int wfd;
};

class Controller {
    public:
    Controller(int num, int max);
    int getNumSwitches();
    void openConnection(char swiID);
    void openReadFIFO(int swID);
    void initializeConnection(int swID);

    private:
    std::string name;
    int nSwitches;
    Connection conns[MAX_NSW];
};