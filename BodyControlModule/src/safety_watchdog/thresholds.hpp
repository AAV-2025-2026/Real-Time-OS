/*
 * thresholds.h
 *
 *  Created on: Dec 1, 2025
 *      Author: mhasa
 */

#ifndef THRESHOLDS_HPP_
#define THRESHOLDS_HPP_
#include <chrono>
#include <mutex>

// Hardware Watchdog
#define MAX_SPEED 10.0f					//Meters per seconds
#define MIN_BATTERY_VOLTAGE 10.0f		//Voltage
// #define LIDAR_MIN_POINTS 50			//LIDAR

// Software Watchdog
#define PROCESS_HEARTBEAT_TIMEOUT 100	//Milliseconds
#define SENSOR_HEARTBEAT_TIMEOUT 100	//Milliseconds

struct SensorState {
    float current_speed;
    float battery_voltage;
    uint64_t last_speed_update_ms;
    std::mutex data_mutex;
};

struct TaskState {
    uint64_t last_sensor_heartbeat;
    uint64_t last_command_heartbeat;
    std::mutex state_mutex;
};

#endif /* THRESHOLDS_HPP_ */
