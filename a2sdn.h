

struct flow_entry {
    uint srcIP_lo;
    uint srcIP_hi;
    uint destIP_lo;
    uint destIP_hi;
    uint actionType;
    uint actionVal;
    uint pri;
    uint pktCount;
};