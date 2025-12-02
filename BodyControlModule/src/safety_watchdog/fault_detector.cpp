/*
 * fault_detector.cpp
 *
 *  Created on: Dec 1, 2025
 *      Author: mhasa
 */
#include <stdio.h>
#include <stdbool.h>
#include "thresholds.hpp"

extern void execute_immediate_halt(const char* reason);
extern bool are_tasks_healthy(char* error_msg_buffer);
extern TaskState current_task_state;
extern SensorState current_sensor_state;

extern struct SensorState current_sensor_state;

void run_safety_check_loop() {

	{
		std::lock_guard<std::mutex> lock(current_task_state.state_mutex);
		char error_buffer[100];

		if (!are_tasks_healthy(error_buffer)) {
			execute_immediate_halt(error_buffer);
			return;
		}
	}

	{
		std::lock_guard<std::mutex> lock(current_sensor_state.data_mutex);

		if (current_sensor_state.current_speed > MAX_SPEED) {
			execute_immediate_halt("FD: Speed limit reached");
			return;
		}

		if (current_sensor_state.battery_voltage < MIN_BATTERY_VOLTAGE) {
			execute_immediate_halt("FD: Minimum battery voltage reached");
			return;
		}
	}

	// repeat for any other types of sensor states
    printf("System Status: NOMINAL\n");
}
