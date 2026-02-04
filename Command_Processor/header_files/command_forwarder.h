#ifndef HEADER_FILES_COMMAND_FORWARDER_H
#define HEADER_FILES_COMMAND_FORWARDER_H

#include "types.h"
#include "priority_selector.h"
#include <memory>
#include <thread>
#include <atomic>
#include <functional>

namespace command_processor {

/**
 * @brief Command Forwarder
 * 
 * Real-time task that forwards selected commands to the Body Control Module
 * at a fixed, deterministic rate (10ms period).
 * 
 */
class CommandForwarder {
public:
    using ForwardCallback = std::function<void(const Command&)>;

    explicit CommandForwarder(std::shared_ptr<PrioritySelector> priority_selector);
    ~CommandForwarder();

    /**
     * @brief Start the forwarding task
     * @param callback Function to call with command to forward to BCM
     */
    void start(ForwardCallback callback);

    /**
     * @brief Stop the forwarding task
     */
    void stop();

    /**
     * @brief Check if forwarder is running
     */
    bool is_running() const;

    /**
     * @brief Forwarding statistics
     */
    struct Statistics {
        uint64_t commands_forwarded;
        uint64_t no_command_cycles;
        uint32_t avg_loop_time_us;
        uint32_t max_loop_time_us;
        std::chrono::steady_clock::time_point last_forward_time;
    };
    
    Statistics get_statistics() const;

    /**
     * @brief Configuration for forwarder behavior
     */
    struct Config {
        std::chrono::milliseconds forward_period = COMMAND_FORWARD_PERIOD_MS;
        bool send_heartbeat_on_no_command = true;  // Send empty/safe command if no valid input
    };
    
    void set_config(const Config& config);

private:
    /**
     * @brief Main forwarding loop (runs in separate thread)
     */
    void forwarding_loop();

    /**
     * @brief Calculate loop timing statistics
     */
    void update_timing_stats(uint32_t loop_time_us);

    std::shared_ptr<PrioritySelector> priority_selector_;
    ForwardCallback forward_callback_;
    
    std::unique_ptr<std::thread> forwarding_thread_;
    std::atomic<bool> running_{false};
    
    Config config_;
    Statistics stats_;
    mutable std::mutex stats_mutex_;
};

} // namespace command_processor

#endif // HEADER_FILES_COMMAND_FORWARDER_H
