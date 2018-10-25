/*
 a2sdn

 @author Ryan Furrer (rfurrer)
*/

/*
In this assignment, you are asked to write a C/C++ program, called a2sdn, that implements the
transactions performed by a simple linear SDN. The program can be invoked as a controller using
"a2sdn cont nSwitch" where cont is a reserved word, and nSwitch specifies the
number of switches in the network (at most MAX NSW= 7 switches). The program can also
be invoked as a switch using "a2sdn swi trafficFile [null|swj] [null|swk]
IPlow-IPhigh". In this form, the program simulates switch swi by processing traffic read
from file trafficFile. Port 1 and port 2 of swi are connected to switches swj and swk,
respectively. Either, or both, of these two switches may be null. Switch swi handles traffic from
hosts in the IP range [IPlow-IPhigh]. Each IP address is ≤ MAXIP (= 1000).
Data transmissions among the switches and the controller use FIFOs. Each FIFO is named
fifo-x-y where x 6= y, and x = 0 (or, y = 0) for the controller, and x, y ∈ [1, MAX NSW] for a
switch. Thus, e.g., sw2 sends data to the controller on fifo-2-0.
*/

//TODO : Add warning and fatal error handling
/*
stdio.h, stdlib.h, string.h, unistd.h, errno.h, stdarg.h, fcntl.h
WARNING ("wrong number of arguments (= %d) \n", argc);
FATAL ("unable to open file (errno= %d): %s \n", errno, strerror(errno));
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


void cmd_exit(){

}

int cntr_loop(Controller controller){
    printf("Controller\n");
    unsigned int i = 1;
    for(;;) {

  	   printf("Please enter 'list' or 'exit': ");
       if (i == 1) return 0;
    }
}


int swi_loop(Switch SDNswitch){
    printf("Switch\n");
    unsigned int i = 1;
    for(;;) {
      /*1. Read and process a single line from the traffic line (ifthe EOF has
      not been reached yet). The switch ignores empty lines, comment lines,
      and lines specifying other handling switches. A packet header is
      considered admitted if the line specifies the current switch.
      */

      /*
      2. Poll the keyboard for a user command. The user can issue one of
      the following commands.
      • list: The program writes all entries in the flow table, and
      for each transmitted or received
      packet type, the program writes an aggregate count of
      handled packets of this type.
      • exit: The program writes the above information and exits.
      */
  	   printf("Please enter 'list' or 'exit': ");
       if (i == 1) return 0;
    }
}

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

    if (argc == 3 && std::strcmp(argv[1], "cont") == 0){
      //setup Controller

        signal(SIGUSR1, user1_Controller);

        Controller controller(atoi(argv[2]));

        ptrController = &controller;
        return cntr_loop(controller);

    } else if (argc < 6){
        printf("Missing arguments\n");
        return 1;

    } else if (argc == 6){

      signal(SIGUSR1, user1_Switch);

      char swi[128];
      strcpy(swi, argv[1]);

      if (swi[0] == 's' && swi[1] == 'w'){ //setup switch
        char* IPlow = strtok (argv[5],"-");
        char* IPhigh = strtok (NULL,"-");

        Switch SDNswitch(swi[2], argv[2], atoi(IPlow), atoi(IPhigh));
        SDNswitch.setPorts(argv[3], argv[4]);

        ptrSwitch = &SDNswitch;

        return swi_loop(SDNswitch);
      }

      printf("Switch names must follow 'swi' format where i is an int.\n");
      return 1;

    } else {
        printf("Too many arguments\n");
        return 1;
    }
}
