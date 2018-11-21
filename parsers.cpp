#include "parsers.h"
#include <stdio.h>
#include <unistd.h>

T_TYPES getTrafficFileLineType(string &line) {
  if (line.length() < 4 || line.substr(0, 2) != "sw") {
    return INVALID;
  }

  istringstream iss(line);
  vector<string> items((istream_iterator<string>(iss)), istream_iterator<string>());

  if (items.size() == 3 && items.at(1) == "delay")
      return DELAY;
  if (items.size() == 3)
      return ROUTE;
  return INVALID;
}

RoutePacket parseTrafficRouteLine(string &line) {
  istringstream iss(line);
  vector<string> items((istream_iterator<string>(iss)), istream_iterator<string>());

  RoutePacket packet;
  packet.swiID = stoi(items.at(0).substr(2,2));
  packet.srcIP = atoi(items.at(1).c_str());
  packet.dstIP = atoi(items.at(2).c_str());
  return packet;
}

DelayPacket parseTrafficDelayLine(string &line) {
  istringstream iss(line);
  vector<string> items((istream_iterator<string>(iss)), istream_iterator<string>());

  DelayPacket packet;
  packet.swiID = stoi(items.at(0).substr(2,2));
  packet.delay = atoi(items.at(2).c_str());
  return packet;
}

int parseAddress(const char* id, const char* servAddress, const char* portNum,
                struct addrinfo *hints, struct addrinfo **res) {
  /* Converst and address into a socket and returns the fd */
  memset((char*)hints, 0, sizeof(*hints));
	hints->ai_family = AF_INET;
	hints->ai_socktype = SOCK_STREAM;
  printf("Trying to connect to %s:%s\n", servAddress, portNum);

	getaddrinfo(servAddress, portNum, hints, res);
	int sfd = socket((**res).ai_family, (**res).ai_socktype, (**res).ai_protocol);

  if (connect(sfd, (**res).ai_addr, (**res).ai_addrlen) < 0) {
		perror("Switch Could Not Connect to Server");
		exit(EXIT_FAILURE);
	}
  send(sfd , id, strlen(id), 0);
  printf("I be connected the sockpuppet.\n" );
  return sfd;
}

int parsePort(int maxSwi, const char* portNum, struct addrinfo *hints,
              struct addrinfo **res) {
	memset((char*)hints, 0, sizeof(*hints));
	hints->ai_family = AF_INET;
	hints->ai_socktype = SOCK_STREAM;
	hints->ai_flags = AI_NUMERICSERV | AI_PASSIVE;

	getaddrinfo(NULL, portNum, hints, res);
	int sfd = socket((**res).ai_family, (**res).ai_socktype, (**res).ai_protocol);

	if (bind(sfd, (**res).ai_addr, (**res).ai_addrlen) < 0) {
		perror("Could not bind");
		exit(EXIT_FAILURE);
	}


	if (listen(sfd, maxSwi) < 0) {
		perror("Could not set socket to listening socket");
		exit(EXIT_FAILURE);
	}
  printf("I be listening to the sockpuppet.\n" );
  return sfd;
}
