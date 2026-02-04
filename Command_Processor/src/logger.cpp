#include "../header_files/logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace command_processor {

// ConsoleLogger Implementation

ConsoleLogger::ConsoleLogger() : verbose_(true) {}

std::string ConsoleLogger::get_timestamp_string() const {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % std::chrono::milliseconds(1000ms);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

void ConsoleLogger::log_command_received(const Command& cmd) {
    if (verbose_) {
        std::cout << "[" << get_timestamp_string() << "] "
                  << "[INTAKE] Command received from "
                  << command_source_to_string(cmd.source)
                  << " (seq: " << cmd.sequence_number << ")"
                  << std::endl;
    }
}

void ConsoleLogger::log_validation(const Command& cmd, const ValidationMetadata& validation) {
    std::cout << "[" << get_timestamp_string() << "] "
              << "[VALIDATOR] Command from "
              << command_source_to_string(cmd.source)
              << " - Result: " << validation_result_to_string(validation.result);
    
    if (validation.result != ValidationResult::VALID) {
        std::cout << " - Reason: " << validation.reason;
    }
    
    std::cout << std::endl;
}

void ConsoleLogger::log_priority_selection(CommandSource selected_source, const Command& cmd) {
    if (verbose_) {
        std::cout << "[" << get_timestamp_string() << "] "
                  << "[SELECTOR] Selected command from "
                  << command_source_to_string(selected_source)
                  << " (seq: " << cmd.sequence_number << ")"
                  << std::endl;
    }
}

void ConsoleLogger::log_command_forwarded(const Command& cmd) {
    if (verbose_) {
        std::cout << "[" << get_timestamp_string() << "] "
                  << "[FORWARDER] Forwarded command from "
                  << command_source_to_string(cmd.source)
                  << " - Steering: " << cmd.sensor_data.steering_angle
                  << "degrees, Speed: " << cmd.sensor_data.speed << " m/s"
                  << std::endl;
    }
}

void ConsoleLogger::log_watchdog_heartbeat() {
    // Don't log every heartbeat (too verbose)
    // Could add a counter to log every Nth heartbeat if needed
}

void ConsoleLogger::log_state_transition(SystemState from, SystemState to, const std::string& reason) {
    std::cout << "[" << get_timestamp_string() << "] "
              << "[STATE] Transition: " << static_cast<int>(from)
              << " -> " << static_cast<int>(to)
              << " - Reason: " << reason
              << std::endl;
}

void ConsoleLogger::log_error(const std::string& component, const std::string& message) {
    std::cerr << "[" << get_timestamp_string() << "] "
              << "[ERROR] [" << component << "] "
              << message
              << std::endl;
}

void ConsoleLogger::log_info(const std::string& component, const std::string& message) {
    std::cout << "[" << get_timestamp_string() << "] "
              << "[INFO] [" << component << "] "
              << message
              << std::endl;
}

} // namespace command_processor
