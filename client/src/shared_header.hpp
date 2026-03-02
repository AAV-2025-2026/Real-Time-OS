#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <sys/neutrino.h>

#define ATTACH_POINT "my_qnx_server"

#define WATCHDOG_PRIORITY 15
#define CLIENT_PRIORITY 10

#define WATCHDOG_PULSE_CODE (_PULSE_CODE_MINAVAIL + 1)
#define HEARTBEAT_THRESHOLD_S 7

typedef union {
    struct _pulse pulse;
    /* Add other message types here if needed */
    char padding[1024]; // Safety buffer
} my_message_t;
