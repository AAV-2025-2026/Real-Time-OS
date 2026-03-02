#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <sys/iofunc.h>
#include <iostream>

#define ATTACH_POINT "my_qnx_server"
#define PULSE_HEARTBEAT (_PULSE_CODE_MINAVAIL + 1)

#define MY_SYSTEM_ID 101
#define CLIENT_PRIORITY 10


int main() {
	int coid;

    if ((coid = name_open(ATTACH_POINT, 0)) == -1) {
    	std::cerr << "Failed to find Watchdog Manager.\n";
    	return EXIT_FAILURE;
	}
    std::cout << "Pulse Client (ID: " << MY_SYSTEM_ID << ") connected. Sending heartbeats...\n";

    while (1) {
    	MsgSendPulse(coid, CLIENT_PRIORITY, PULSE_HEARTBEAT, MY_SYSTEM_ID); // server id, priority, 8-bit pulse code, 32-bit pulse value
    	usleep(50000);	// 500ms
    }

    name_close(coid);

    return 0;
}
