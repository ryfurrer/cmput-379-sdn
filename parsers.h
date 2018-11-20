#ifndef _PARSERS_
#define _PARSERS_

#include <tuple>

using namespace std;


// Traffic line types
typedef enum {INVALID, ROUTE, DELAY} T_TYPES;
typedef struct {
	int swiID;
	int srcIP;
	int dstIP;
} RoutePacket;

typedef struct {
	int swiID;
	clock_t dalay;
} DelayPacket;

int getTrafficFileLineType(string &line);

RoutePacket parseTrafficRouteItem(string &line);

DelayPacket parseTrafficDelayItem(string &line);

#endif