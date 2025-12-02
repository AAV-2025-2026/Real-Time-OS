/*
 * ros2_interface.cpp
 *
 *  Created on: Dec 1, 2025
 *      Author: mhasa
 */
#include "rclcpp/rclcpp.hpp"					//Comes with ros2 installation
#include "std_msgs/msg/float32.hpp"				//Comes with ros2 installation
#include "thresholds.hpp"

extern void run_safety_check_loop();

extern SensorState current_sensor_state;

uint64_t get_system_time_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

class Watchdog : public rclcpp::Node {
	public:
		Watchdog() : Node("safety_watchdog") {
			timer_ = this->create_wall_timer(50ms, std::bind(&Watchdog::timer_callback, this));

			// Using the following format you can add more types of data to look for
			// Monitor Speed
			speed_monitor = this->create_subscription<std_msgs::msg::Float32(
				"/ros_data/speed",
				10,
				[this](const std_msgs::msg::Float32::SharedPtr msg) {
					std::lock_guard<std::mutex> lock(current_sensor_state.data_mutex);
					current_sensor_state.current_speed = msg->data;
					current_sensor_state.last_speed_update_ms = get_system_time_ms();
				}
			);

			// Monitor Power/Battery
			battery_monitor = this->create_subscription<std_msgs::msg::Float32(
				"/ros_data/battery/voltage",
				10,
				[this](const std_msgs::msg::Float32::SharedPtr msg) {
					std::lock_guard<std::mutex> lock(current_sensor_state.data_mutex);
					current_sensor_state.battery_voltage = msg->data;
				}
			);

		}

	private:

		void timer_callback() {
		        run_safety_check_loop();
		}
		rclcpp::TimerBase::SharedPtr timer_;
		rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr speed_monitor;
		rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr battery_monitor;
};
