#include <iostream>
#include <map>
#include <vector>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#define PULSE_HEARTBEAT      (_PULSE_CODE_MINAVAIL + 1)
#define PULSE_WATCHDOG_TIMER (_PULSE_CODE_MINAVAIL + 2)

#define ATTACH_POINT "my_qnx_server"

struct ProcessData {
    bool systemStatus; 	// True if heartbeat received during current cycle
    bool isActive; 		// True if system is permanently active/registered
};

void KickHardwareWatchdog() {
	// Kick the motor control board to engage the safety
    std::cout << "[Watchdog], HW Watchdog Kicked. All systems normal.\n";
}

void TriggerSystemRecovery(int failed_system_id) {
	// Timeout the watchdog timer on the control board initiating the emergency system
	std::cout << "[Watchdog], HW Watchdog Timed Out, System ID " << failed_system_id << " is stuck!\n";
}

int main() {
    struct _pulse pulse;
	name_attach_t *attach;
	std::map<int, ProcessData> statusTable; // pid, system status, system active or not?
    std::vector<int> monitoredSystems = {101};//, 102, 103};
    // Needs to be equal to the number of process being tracked
    // Each id being unique to each process
    for (int sys_id : monitoredSystems) {
    	statusTable[sys_id] = {false,false};
    }


    if ((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL) return EXIT_FAILURE;
    int coid = ConnectAttach(0, 0, attach->chid, _NTO_SIDE_CHANNEL, 0);

    struct sigevent timer_event;
    SIGEV_PULSE_INIT(&timer_event, coid, SIGEV_PULSE_PRIO_INHERIT, PULSE_WATCHDOG_TIMER, 0);

    timer_t timer_id;
    timer_create(CLOCK_MONOTONIC, &timer_event, &timer_id);

    // Set the timer to fire every 5 second (for ease of testing)
    struct itimerspec itime;
    itime.it_value.tv_sec = 0;
    itime.it_value.tv_nsec = 200000000;
    itime.it_interval.tv_sec = 0;
    itime.it_interval.tv_nsec = 200000000;
    timer_settime(timer_id, 0, &itime, NULL);

    std::cout << "Watchdog Manager started. Listening for pulses...\n";

    while (true) {
        int rcvid = MsgReceive(attach->chid, &pulse, sizeof(pulse), NULL);

        if (rcvid == -1) {
            std::cerr << "MsgReceive failed.\n";
            break;
        }

        if (rcvid == 0) {
            switch (pulse.code) {
                // Heartbeat from internal systems
                case PULSE_HEARTBEAT: {
                    int sender_id = pulse.value.sival_int;
                    if (statusTable.count(sender_id)) {
                        statusTable[sender_id].isActive = true;
                        statusTable[sender_id].systemStatus = true;
                        std::cout << "Heartbeat received from " << sender_id << "\n";
                    }
                    break;
                }
                // Internal timer firing
                case PULSE_WATCHDOG_TIMER: {
                    bool all_ok = true;
                    int failed_id = -1;

                    for (auto& pair : statusTable) {
                        if (!pair.second.systemStatus) {
                            all_ok = false;
                            failed_id = pair.first;
                            break;
                        }
                    }

                    // The Logic
                    if (all_ok) {
                        KickHardwareWatchdog();
                        for (auto& pair : statusTable) {
                            pair.second = {false, true};
                        }
                    } else {
                        TriggerSystemRecovery(failed_id);
                    }
                    break;
                }
                case _PULSE_CODE_DISCONNECT:
					ConnectDetach(pulse.scoid);
					break;
                default:
                    // Ignore unknown pulses
                    break;
            }
        } else {
            MsgError(rcvid, ENOSYS);
        }
    }

    return 0;
}
