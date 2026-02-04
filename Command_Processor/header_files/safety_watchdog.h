#ifndef HEADER_FILES_SAFETY_WATCHDOG_H
#define HEADER_FILES_SAFETY_WATCHDOG_H

#include "types.h"
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>

namespace command_processor {

/**
 * @brief Safety Watchdog Heartbeat Generator
 * 
 * Sends periodic heartbeats to external system safety watchdog component.
 * The external watchdog handles all timeout detection and emergency stop logic.
 * 
 */
class SafetyWatchdog {
public:
    SafetyWatchdog();
    ~SafetyWatchdog();

    /**
     * @brief Start sending heartbeats to external watchdog
     * @param heartbeat_callback Function to send heartbeat pulse to external watchdog component
     */
    void start(std::function<void()> heartbeat_callback);

    /**
     * @brief Stop sending heartbeats
     */
    void stop();

    /**
     * @brief Feed the watchdog (call from command processing pipeline)
     * 
     * This should be called by the command processor to indicate it's alive and processing.
     * Resets the internal timer so heartbeats continue.
     */
    void feed();

    /**
     * @brief Check if heartbeat generator is running
     */
    bool is_running() const;

    /**
     * @brief Configuration
     */
    struct Config {
        std::chrono::milliseconds heartbeat_period_ms = std::chrono::milliseconds(50ms);  // How often to send heartbeat to external watchdog
    };
    
    void set_config(const Config& config);

    /**
     * @brief Heartbeat statistics
     */
    struct Statistics {
        uint64_t heartbeats_sent;
        uint64_t feeds_received;
        uint32_t time_since_last_feed_ms;
    };
    
    Statistics get_statistics() const;

private:
    /**
     * @brief Main heartbeat loop
     */
    void heartbeat_loop();

    std::function<void()> heartbeat_callback_;
    
    std::unique_ptr<std::thread> heartbeat_thread_;
    std::atomic<bool> running_{false};
    
    std::chrono::steady_clock::time_point last_feed_time_;
    mutable std::mutex feed_mutex_;
    
    Config config_;
    Statistics stats_;
    mutable std::mutex stats_mutex_;
};

} // namespace command_processor

#endif // HEADER_FILES_SAFETY_WATCHDOG_H