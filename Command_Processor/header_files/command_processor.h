#ifndef HEADER_FILES_HEADER_FILES_H
#define HEADER_FILES_HEADER_FILES_H

#include "types.h"
#include "command_intake.h"
#include "command_validator.h"
#include "latest_command_slot.h"
#include "priority_selector.h"
#include "command_forwarder.h"
#include "safety_watchdog.h"
#include "logger.h"

#include <memory>
#include <functional>

namespace command_processor {

/**
 * @brief Main Command Processor Orchestrator
 * 
 * Integrates all command processing components and manages the overall
 * command flow from intake to BCM forwarding.
 * 
 * This is the main interface that external systems (ROS2 nodes) interact with.
 */
class CommandProcessor {
public:
    /**
     * @brief Callback types for external integration
     */
    using BCMCallback = std::function<void(const Command&)>;
    using WatchdogCallback = std::function<void()>;

    /**
     * @brief Constructor
     * @param logger Logger implementation (optional, uses ConsoleLogger if nullptr)
     */
    explicit CommandProcessor(std::shared_ptr<Logger> logger = nullptr);
    ~CommandProcessor();

    /**
     * @brief Initialize the command processor
     * @param bcm_callback Callback to send commands to BCM
     * @param watchdog_callback Callback to send heartbeat to external system watchdog
     * @return true if initialization successful
     */
    bool initialize(BCMCallback bcm_callback,
                   WatchdogCallback watchdog_callback);

    /**
     * @brief Start command processing
     * Starts the forwarding loop and watchdog heartbeat
     */
    void start();

    /**
     * @brief Stop command processing
     */
    void stop();

    /**
     * @brief Process incoming command from external source
     * 
     * This is the main entry point for commands.
     * 
     * @param source Command source
     * @param sensor_data Sensor data payload
     * @param sequence_number Sequence number from source
     */
    void process_command(CommandSource source,
                        const Command::SensorData& sensor_data,
                        uint64_t sequence_number);

    /**
     * @brief Get current system state
     */
    SystemState get_state() const;

    /**
     * @brief Manually trigger emergency stop
     */
    void trigger_emergency_stop();

    /**
     * @brief Reset to normal operation after fault recovery
     * @return true if reset successful
     */
    bool reset();

    /**
     * @brief Configuration structure
     */
    struct Config {
        CommandValidator::Config validator_config;
        CommandForwarder::Config forwarder_config;
        SafetyWatchdog::Config watchdog_config;
    };
    
    void set_config(const Config& config);

    /**
     * @brief Get comprehensive statistics
     */
    struct Statistics {
        CommandIntake::Statistics intake_stats;
        PrioritySelector::Statistics selector_stats;
        CommandForwarder::Statistics forwarder_stats;
        SafetyWatchdog::Statistics watchdog_stats;
        SystemState current_state;
    };
    
    Statistics get_statistics() const;

private:
    /**
     * @brief Internal command processing pipeline
     */
    void handle_command(const Command& cmd);

    /**
     * @brief Handle validation result
     */
    void handle_validation_result(const Command& cmd, const ValidationMetadata& validation);

    /**
     * @brief Transition system state
     */
    void transition_state(SystemState new_state, const std::string& reason);

    // Core components
    std::shared_ptr<Logger> logger_;
    std::unique_ptr<CommandIntake> intake_;
    std::unique_ptr<CommandValidator> validator_;
    std::shared_ptr<LatestCommandSlot> command_slot_;
    std::shared_ptr<PrioritySelector> priority_selector_;
    std::unique_ptr<CommandForwarder> forwarder_;
    std::unique_ptr<SafetyWatchdog> watchdog_;

    // External callbacks
    BCMCallback bcm_callback_;
    WatchdogCallback watchdog_callback_;

    // State management
    SystemState current_state_;
    mutable std::mutex state_mutex_;

    bool initialized_;
};

} // namespace command_processor

#endif // HEADER_FILES_HEADER_FILES_H