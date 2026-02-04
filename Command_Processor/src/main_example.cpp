#include "../header_files/command_processor.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace command_processor;

/**
 * Example demonstration of Command Processor usage
 * 
 * This shows how to:
 * 1. Initialize the command processor
 * 2. Set up callbacks for BCM and watchdog
 * 3. Send commands from different sources
 * 4. Monitor statistics
 */

// Simulated BCM callback
void send_to_bcm(const Command& cmd) {
    std::cout << "\n=== BCM RECEIVED COMMAND ===" << std::endl;
    std::cout << "Source: " << command_source_to_string(cmd.source) << std::endl;
    std::cout << "Sequence: " << cmd.sequence_number << std::endl;
    std::cout << "Steering: " << cmd.sensor_data.steering_angle << "°" << std::endl;
    std::cout << "Speed: " << cmd.sensor_data.speed << " m/s" << std::endl;
    std::cout << "Acceleration: " << cmd.sensor_data.acceleration << " m/s²" << std::endl;
    std::cout << "Brake: " << (cmd.sensor_data.brake_engaged ? "ENGAGED" : "RELEASED") << std::endl;
    std::cout << "===========================\n" << std::endl;
}

// Simulated system watchdog callback
void watchdog_heartbeat() {
    // In real system, this would pulse the hardware watchdog
    // The external watchdog component handles all timeout and emergency stop logic
    // std::cout << "[WATCHDOG] Heartbeat sent to external watchdog" << std::endl;
}

void print_statistics(const CommandProcessor& processor) {
    auto stats = processor.get_statistics();
    
    std::cout << "\n--- Command Processor Statistics ---" << std::endl;
    std::cout << "Commands received: " << stats.intake_stats.commands_received << std::endl;
    std::cout << "Safety selections: " << stats.selector_stats.safety_selections << std::endl;
    std::cout << "Manual selections: " << stats.selector_stats.manual_selections << std::endl;
    std::cout << "Remote selections: " << stats.selector_stats.remote_selections << std::endl;
    std::cout << "Autonomous selections: " << stats.selector_stats.autonomous_selections << std::endl;
    std::cout << "Commands forwarded: " << stats.forwarder_stats.commands_forwarded << std::endl;
    std::cout << "Watchdog heartbeats: " << stats.watchdog_stats.heartbeats_sent << std::endl;
    std::cout << "Watchdog feeds: " << stats.watchdog_stats.feeds_received << std::endl;
    std::cout << "------------------------------------\n" << std::endl;
}

int main() {
    using namespace std::chrono_literals;
    auto duration = std::chrono::milliseconds(100ms);
    auto sleep_time = std::chrono::seconds(1s);
    
    std::cout << "Command Processing Module - Example Program" << std::endl;
    std::cout << "=========================================\n" << std::endl;
    
    // Create command processor
    std::cout << "Initializing command processor..." << std::endl;
    auto processor = std::make_unique<CommandProcessor>(std::make_shared<ConsoleLogger>());
    std::cout << "Command processor initialized.\n" << std::endl;

    // Initialize with callbacks (no emergency callback - handled by external watchdog)
    if (!processor->initialize(send_to_bcm, watchdog_heartbeat)) {
        std::cerr << "Failed to initialize command processor" << std::endl;
        return 1;
    }
    
    // Start processing
    processor->start();
    std::cout << "Starting command processor..." << std::endl;
    std::cout << "\nCommand processor started. Sending test commands...\n" << std::endl;
    
    // Simulate commands from different sources
    uint64_t sequence = 0;
    
    // Test 1: Remote command
    std::cout << "Test 1: Sending remote control command" << std::endl;
    Command::SensorData remote_data;
    remote_data.steering_angle = 15.0f;
    remote_data.speed = 5.0f;
    remote_data.acceleration = 0.5f;
    remote_data.brake_engaged = false;
    processor->process_command(CommandSource::REMOTE, remote_data, ++sequence);
    std::this_thread::sleep_for(duration);
    
    // Test 2: Manual override (higher priority)
    std::cout << "Test 2: Sending manual control command (should override remote)" << std::endl;
    Command::SensorData manual_data;
    manual_data.steering_angle = -10.0f;
    manual_data.speed = 3.0f;
    manual_data.acceleration = 0.0f;
    manual_data.brake_engaged = false;
    processor->process_command(CommandSource::MANUAL, manual_data, ++sequence);
    std::this_thread::sleep_for(duration);
    
    // Test 3: Safety command (highest priority)
    std::cout << "Test 3: Sending safety command (should override everything)" << std::endl;
    Command::SensorData safety_data;
    safety_data.steering_angle = 0.0f;
    safety_data.speed = 0.0f;
    safety_data.acceleration = -2.0f;
    safety_data.brake_engaged = true;
    processor->process_command(CommandSource::SAFETY, safety_data, ++sequence);
    std::this_thread::sleep_for(duration);
    
    // Test 4: Autonomous command (lowest priority, won't be used while others active)
    std::cout << "Test 4: Sending autonomous command" << std::endl;
    Command::SensorData auto_data;
    auto_data.steering_angle = 5.0f;
    auto_data.speed = 8.0f;
    auto_data.acceleration = 0.3f;
    auto_data.brake_engaged = false;
    processor->process_command(CommandSource::AUTONOMOUS, auto_data, ++sequence);
    std::this_thread::sleep_for(duration);
    
    // Test 5: Invalid command (out of range steering)
    std::cout << "Test 5: Sending invalid command (steering out of range)" << std::endl;
    Command::SensorData invalid_data;
    invalid_data.steering_angle = 100.0f;  // Exceeds max_steering_angle
    invalid_data.speed = 5.0f;
    invalid_data.acceleration = 0.0f;
    invalid_data.brake_engaged = false;
    processor->process_command(CommandSource::REMOTE, invalid_data, ++sequence);
    std::this_thread::sleep_for(duration);
    
    // Let the forwarder run for a bit
    std::cout << "\nLetting system run for 1 second..." << std::endl;
    std::this_thread::sleep_for(sleep_time);
    
    // Print statistics
    print_statistics(*processor);
    
    // Stop the processor
    std::cout << "Stopping command processor..." << std::endl;
    processor->stop();
    
    std::cout << "\nExample complete!" << std::endl;
    
    return 0;
}
