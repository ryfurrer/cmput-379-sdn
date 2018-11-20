#ifndef _PARSERS_
#define _PARSERS_

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

using namespace std;


// Traffic line types
typedef enum {INVALID, DELAY, ROUTE} T_TYPES;
typedef struct {
	int swiID;
	int srcIP;
	int dstIP;
} RoutePacket;

typedef struct {
	int swiID;
	clock_t delay;
} DelayPacket;

T_TYPES getTrafficFileLineType(string &line);
RoutePacket parseTrafficRouteItem(string &line);
DelayPacket parseTrafficDelayItem(string &line);

int parseAddress(const char* servAddress, const char* portNum,
                struct addrinfo *hints, struct addrinfo **res);
int parsePort(int maxSwi, const char* portNum, struct addrinfo *hints,
							struct addrinfo **res);
#endif
