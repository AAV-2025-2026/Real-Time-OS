/*
 * estop_protocol.cpp
 *
 *  Created on: Dec 1, 2025
 *      Author: mhasa
 */
#include <stdio.h>
#include "thresholds.hpp"

static bool is_halted = false;

void execute_immediate_halt(const char* reason) {

	if (!is_halted){
		printf("EMERGENCY STOP TRIGGERED: %s\n", reason);
    	is_halted = true;
		/*
		 * I don't know what this would look like
		 */
        while(1) {
        	// Implement a software based way to go back to operating as usual
        	// Only valid if both tasks are running
        }
	}
}
