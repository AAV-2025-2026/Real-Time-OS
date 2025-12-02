/*
 * task_interface.cpp
 *
 *  Created on: Dec 1, 2025
 *      Author: mhasa
 */
#include "thresholds.hpp"
#include <cstdio>

using namespace std::chrono;

SensorState current_sensor_state;
TaskState current_task_state;

uint64_t get_system_time_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

void init_task_monitoring() {
    std::lock_guard<std::mutex> lock(current_task_state.state_mutex);
    uint64_t now = get_system_time_ms();
    current_task_state.last_sensor_heartbeat = now;
    current_task_state.last_command_heartbeat = now;
}

void update_sensor_heartbeat(uint64_t time) {
    std::lock_guard<std::mutex> lock(current_task_state.state_mutex);
    current_task_state.last_sensor_heartbeat = time;
}

void update_command_heartbeat(uint64_t time) {
    std::lock_guard<std::mutex> lock(current_task_state.state_mutex);
    current_task_state.last_command_heartbeat = time;
}

bool are_tasks_healthy(char* error_msg_buffer) {
    std::lock_guard<std::mutex> lock(current_task_state.state_mutex);
    uint64_t now = get_system_time_ms();

    // Check Sensor Task
    uint64_t sensor_diff = now - current_task_state.last_sensor_heartbeat;
    if (sensor_diff > SENSOR_HEARTBEAT_TIMEOUT) {
    	sprintf(error_msg_buffer, "TI: Sensor Processor hung (%lld ms)", sensor_diff);
    	return false;
    }

    // Check Command Task
    uint64_t command_diff = now - current_task_state.last_command_heartbeat;
    if (command_diff > PROCESS_HEARTBEAT_TIMEOUT) {
    	sprintf(error_msg_buffer, "TI: Command Processor hung (%lld ms)", command_diff);
    	return false;
    }

    return true;
}
