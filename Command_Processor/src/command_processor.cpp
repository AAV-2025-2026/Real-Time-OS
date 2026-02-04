#include "../header_files/command_processor.h"

namespace command_processor {

CommandProcessor::CommandProcessor(std::shared_ptr<Logger> logger)
    : current_state_(SystemState::INITIALIZING)
    , initialized_(false) {
    
    // Use provided logger or create ConsoleLogger if none provided
    if (logger) {
        logger_ = std::move(logger);
    } else {
        logger_ = std::make_shared<ConsoleLogger>();
    }
    
    logger_->log_info("CommandProcessor", "Command Processor created");
}

CommandProcessor::~CommandProcessor() {
    stop();
}

bool CommandProcessor::initialize(BCMCallback bcm_callback,
                                  WatchdogCallback watchdog_callback) {
    if (initialized_) {
        logger_->log_error("CommandProcessor", "Already initialized");
        return false;
    }
    
    logger_->log_info("CommandProcessor", "Initializing...");
    
    // Store callbacks
    bcm_callback_ = bcm_callback;
    watchdog_callback_ = watchdog_callback;
    
    // Initialize components
    intake_ = std::make_unique<CommandIntake>();
    validator_ = std::make_unique<CommandValidator>();
    command_slot_ = std::make_shared<LatestCommandSlot>();
    priority_selector_ = std::make_shared<PrioritySelector>(command_slot_);
    forwarder_ = std::make_unique<CommandForwarder>(priority_selector_);
    watchdog_ = std::make_unique<SafetyWatchdog>();
    
    // Set up command intake callback
    intake_->set_command_callback([this](const Command& cmd) {
        this->handle_command(cmd);
    });
    
    initialized_ = true;
    transition_state(SystemState::NORMAL_OPERATION, "Initialization complete");
    
    logger_->log_info("CommandProcessor", "Initialization successful");
    return true;
}

void CommandProcessor::start() {
    if (!initialized_) {
        logger_->log_error("CommandProcessor", "Cannot start - not initialized");
        return;
    }
    
    logger_->log_info("CommandProcessor", "Starting command processing...");
    
    // Start command forwarder
    forwarder_->start([this](const Command& cmd) {
        // Forward to BCM
        if (bcm_callback_) {
            bcm_callback_(cmd);
        }
        logger_->log_command_forwarded(cmd);
        
        // Feed the watchdog (indicates command processor is alive)
        watchdog_->feed();
    });
    
    // Start heartbeat generator to external watchdog component
    watchdog_->start(
        // Heartbeat callback - sends pulse to external watchdog
        [this]() {
            if (watchdog_callback_) {
                watchdog_callback_();
            }
            logger_->log_watchdog_heartbeat();
        }
    );
    
    logger_->log_info("CommandProcessor", "Command processing started");
    logger_->log_info("CommandProcessor", "Sending heartbeats to external watchdog component");
}

void CommandProcessor::stop() {
    if (!initialized_) {
        return;
    }
    
    logger_->log_info("CommandProcessor", "Stopping command processing...");
    
    // Stop forwarder first
    if (forwarder_) {
        forwarder_->stop();
    }
    
    // Stop watchdog
    if (watchdog_) {
        watchdog_->stop();
    }
    
    logger_->log_info("CommandProcessor", "Command processing stopped");
}

void CommandProcessor::process_command(CommandSource source,
                                       const Command::SensorData& sensor_data,
                                       uint64_t sequence_number) {
    if (!initialized_) {
        logger_->log_error("CommandProcessor", "Cannot process command - not initialized");
        return;
    }
    
    // Pass to intake for normalization
    intake_->receive_command(source, sensor_data, sequence_number);
}

void CommandProcessor::handle_command(const Command& cmd) {
    // Log received command
    logger_->log_command_received(cmd);
    
    // Validate command
    ValidationMetadata validation = validator_->validate(cmd);
    logger_->log_validation(cmd, validation);
    
    // Handle validation result
    handle_validation_result(cmd, validation);
}

void CommandProcessor::handle_validation_result(const Command& cmd,
                                                const ValidationMetadata& validation) {
    if (validation.result == ValidationResult::VALID) {
        // Store valid command in slot
        command_slot_->store(cmd);
        
        // Priority selector will pick it up on next cycle
        logger_->log_priority_selection(cmd.source, cmd);
    } else {
        // Command rejected - log but don't store
        logger_->log_error("Validator",
                          std::string("Command from ") +
                          command_source_to_string(cmd.source) +
                          " rejected: " + validation.reason);
    }
}

SystemState CommandProcessor::get_state() const {
    std::lock_guard<std::mutex> lock(state_mutex_);
    return current_state_;
}

void CommandProcessor::trigger_emergency_stop() {
    logger_->log_error("CommandProcessor", "Manual emergency stop triggered");
    
    // Clear all commands
    command_slot_->clear_all();
    
    transition_state(SystemState::EMERGENCY_STOP, "Manual trigger");
    
    // Note: Actual emergency stop is handled by external watchdog component
    // We just transition our state and stop processing commands
}

bool CommandProcessor::reset() {
    logger_->log_info("CommandProcessor", "Resetting to normal operation...");
    
    // Clear all stored commands
    command_slot_->clear_all();
    
    // Reset validator state
    validator_->reset();
    
    // Reset statistics
    priority_selector_->reset_statistics();
    
    transition_state(SystemState::NORMAL_OPERATION, "Manual reset");
    
    logger_->log_info("CommandProcessor", "Reset complete");
    return true;
}

void CommandProcessor::set_config(const Config& config) {
    validator_->set_config(config.validator_config);
    forwarder_->set_config(config.forwarder_config);
    watchdog_->set_config(config.watchdog_config);
}

CommandProcessor::Statistics CommandProcessor::get_statistics() const {
    Statistics stats;
    stats.intake_stats = intake_->get_statistics();
    stats.selector_stats = priority_selector_->get_statistics();
    stats.forwarder_stats = forwarder_->get_statistics();
    stats.watchdog_stats = watchdog_->get_statistics();
    stats.current_state = get_state();
    return stats;
}

void CommandProcessor::transition_state(SystemState new_state, const std::string& reason) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    if (current_state_ != new_state) {
        SystemState old_state = current_state_;
        current_state_ = new_state;
        logger_->log_state_transition(old_state, new_state, reason);
    }
}

} // namespace command_processor