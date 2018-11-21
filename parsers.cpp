#include "parsers.h"

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
  printf("Parsing a delay line: %s\n", line.c_str());
  istringstream iss(line);
  vector<string> items((istream_iterator<string>(iss)), istream_iterator<string>());

  DelayPacket packet;
  packet.swiID = stoi(items.at(0).substr(2,2));
  packet.delay = atoi(items.at(2).c_str());
  return packet;
}

int monitorSwitchSocket(int socket) {
  pollSwitch[0].fd = newsocket; //fd for incoming socket
	pollSwitch[0].events = POLLIN;

  while(true){
	   poll(pollSwitch, 1, 0);
	   if ((pollSwitch[0].revents&POLLIN) == POLLIN) {
       if (recv(sfd, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0) {
         //http://www.stefan.buettcher.org/cs/conn_closed.html
         // if recv returns zero, that means the connection has been closed:
         // kill the child process
         printf("Switchy %i be closed\n", 0);
         // TODO: do something
       }
     }
  }
}

int pollControllerSocket(int sfd) {
  int new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  while(1) {
    struct pollfd pollSocket[1], pollSwitch[1];
		pollSocket[0].fd = sfd;
		pollSocket[0].events = POLLIN;

		poll(pollSocket, 1, 0);
		if ((pollSocket[0].revents&POLLIN) == POLLIN) {
      if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
      }

      pid_t pid = fork();
      if (pid == 0) {
        // child process
        //I don't want to change my assiment 2 code so the sockets will
        //be handled in a seperate process
        return monitorSwitchSocket(new_socket);
      } else if (pid > 0) {
          //parent just needs to continue polling
      } else{
        exit(EXIT_FAILURE);
      }
    }
  }
  return EXIT_FAILURE;
}

int parseAddress(const char* servAddress, const char* portNum,
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
  printf("I be connected the sockpuppet.\n" );
  pid_t pid = fork();

  if (pid == 0) {
    // child process
    //I don't want to change my assiment 2 code so the sockets will
    //be handled in a seperate process
    pollControllerSocket(sfd);
  } else if (pid > 0) {
      return sfd;
  }
  exit(EXIT_FAILURE);
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


  return sfd;//http://beej.us/guide/bgnet/html/multi/syscalls.html#accept
}
