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

#include <a2sdn.h>

#define MAX_NSW 7
#define FORWARD 0
#define DROP 1

void list(){
    
}

void cmd_exit(){
    
}

void loop(){
    uint i = 1;
    for(;;) {
    /*1. Read and process a single line from the traffic line (ifthe EOF has not been reached yet). The 
    switch ignores empty lines, comment lines, and lines specifying other handling switches. A
    packet header is considered admitted if the line specifies the current switch.
    */
    
    /*
    2. Poll the keyboard for a user command. The user can issue one of the following commands.
    • list: The program writes all entries in the flow table, and for each transmitted or received
    packet type, the program writes an aggregate count of handled packets of this
    type.
    • exit: The program writes the above information and exits.
    */
        if (i == 1) return 0;
    }
}


int main(int argc, char *argv[]) {
    if (argc < 3){
        printf("Missing arguments\n");
        return 1;
    }
    
    
    if (argv[1] == "cont") {
    } else if (argc == 6){
    } else {
        printf("Too many arguments\n");
        return 1;
    }
    
    loop();
    
}