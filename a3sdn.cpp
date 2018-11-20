/*
 Handles command line input and opens a controller or switch.
 Also handles signal SIGUSR1.

 @author Ryan Furrer (rfurrer)
*/




#include "controller.h"
#include "switch.h"
#include <stdio.h> /* printf */
#include <cstring> /* string compare */
#include <signal.h>
#include <stdlib.h>     /* atoi */

#define VALID 1
#define INVALID 0
#define NUM_SWI_ARGS 8

using namespace std; /*  */

Switch* ptrSwitch;
Controller* ptrController;

void user1_Controller(int signum) {
  /* Handle SIGUSR1 signals sent to the program
  if in Controller mode */
	printf("\n\nUSER1 Signal... \n\n");


	ptrController->print();
	printf("Please type 'list' or 'exit: ");
	fflush(stdout);
}

void user1_Switch(int signum) {
  /* Handle SIGUSR1 signals sent to the program
  if in Switch mode */
	printf("\n\nUSER1 Signal... \n\n");

  ptrSwitch->print();
	printf("Please enter 'list' or 'exit': ");
	fflush(stdout);
}

int valid_for_cont(int argc, char *argv[]) {
    //check valid cli inputs for a controller
    if (argc == 3 && std::strcmp(argv[1], "cont") == 0) {
        int num = atoi(argv[2]);
        if (num < MIN_NSW || num > MAX_NSW) {
          printf("1-7 switches required.\n");
          exit(EXIT_FAILURE);
        }
        return VALID;
    }
    return INVALID;
}

int validate_swi(int argc, char *argv[]) {
    //check valid cli inputs for a switch
    if (argc < NUM_SWI_ARGS){
        printf("Missing arguments\n");
        exit(EXIT_FAILURE);
    } else if (argc == NUM_SWI_ARGS){
      char swi[128];
      strcpy(swi, argv[1]);

      if (swi[0] == 's' && swi[1] == 'w'){
        int IPlow = atoi(strtok (argv[5],"-"));
        int IPhigh = atoi(strtok (NULL,"-"));

        if (IPlow < 0 || IPhigh > MAXIP) {
          printf("invalid ip\n");
          exit(EXIT_FAILURE);
        }
        return VALID;
      }

      printf("Switch names must follow 'swi' format where i is an int.\n");
      exit(EXIT_FAILURE);

    }
    printf("Too many arguments\n");
    exit(EXIT_FAILURE);
}

/**
Sets up signals.
Sets up a switch or controller and then runs them
*/
int main(int argc, char *argv[]) {
		struct addrinfo hints, *res;
		int sfd;

    if (valid_for_cont(argc, argv)){
        signal(SIGUSR1, user1_Controller);
        int numSwitches = atoi(argv[2]);
				sfd = parsePort(numSwitches, argv[3], &hints, &res);

        Controller controller(numSwitches, sfd);
        ptrController = &controller;
        return controller.run();

    } else if (validate_swi(argc, argv)){
        signal(SIGUSR1, user1_Switch);
        char swi[128];
        strcpy(swi, argv[1]);
        int IPlow = atoi(strtok (argv[5],"-"));
        int IPhigh = atoi(strtok (NULL,"-"));
        // string serverAddress = argv[6];
				sfd = parseAddress(argv[6], argv[7], &hints, &res);

        Switch SDNswitch(atoi(&swi[2]), argv[2], IPlow, IPhigh, sfd);
        SDNswitch.setPorts(argv[3], argv[4]);
        ptrSwitch = &SDNswitch;
        return SDNswitch.run();
    }
    return EXIT_FAILURE;
}
