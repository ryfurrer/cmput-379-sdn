#include "flowTable.h"

class Switch {
    public:
    Flow_table flowTable;
    Switch(unsigned int IPlow, unsigned int IPhigh);
    void makeFIFO(const char *pathName);
};
