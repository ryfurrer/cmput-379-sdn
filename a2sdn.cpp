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

int main(int argc, char *argv[]) {
    struct sigaction psa;


    if (argc == 3 && std::strcmp(argv[1], "cont") == 0){
      //setup Controller
        signal(SIGUSR1, user1_Controller);

        int num = atoi(argv[2]);
        if (num < MIN_NSW || num > MAX_NSW) {
          printf("1-7 switches required.\n");
          exit(1);
        }

        Controller controller(atoi(argv[2]));

        ptrController = &controller;
        return controller.run();

    } else if (argc < 6){
        printf("Missing arguments\n");
        return 1;

    } else if (argc == 6){
      signal(SIGUSR1, user1_Switch);

      char swi[128];
      strcpy(swi, argv[1]);

      if (swi[0] == 's' && swi[1] == 'w'){ //setup switch
        int IPlow = atoi(strtok (argv[5],"-"));
        int IPhigh = atoi(strtok (NULL,"-"));

        if (IPlow < 0 || IPhigh > MAXIP) {
          printf("invalid ip\n");
          return 1;
        }

        Switch SDNswitch(atoi(&swi[2]), argv[2], IPlow, IPhigh);
        SDNswitch.setPorts(argv[3], argv[4]);

        ptrSwitch = &SDNswitch;

        return SDNswitch.run();
      }

      printf("Switch names must follow 'swi' format where i is an int.\n");
      return 1;

    } else {
        printf("Too many arguments\n");
        return 1;
    }
}
