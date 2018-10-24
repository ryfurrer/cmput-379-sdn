#ifndef _Flow_Table_
#define  _Flow_Table_

#include <vector>

struct flow_entry {
    unsigned int srcIP_lo;
    unsigned int srcIP_hi;
    unsigned int destIP_lo;
    unsigned int destIP_hi;
    unsigned int actionType;
    unsigned int actionVal;
    unsigned int pri;
    unsigned int pktCount;
};
typedef std::vector<flow_entry> Flow_table;
typedef enum {ACK, OPEN, QUERY, ADD, RELAY} P_TYPES;
typedef enum {DROP, FORWARD, DELIVER } ACTION;

#define MINPRI 4
#define MAXIP 1000

#endif
