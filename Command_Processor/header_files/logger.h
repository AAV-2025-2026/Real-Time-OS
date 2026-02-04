#ifndef HEADER_FILES_LOGGER_H
#define HEADER_FILES_LOGGER_H

#include "types.h"
#include <string>
#include <memory>

namespace command_processor {

/**
 * @brief Logging Interface
 * 
 * Abstract interface for logging command processing events.
 * Implementation can connect to RTOS database, file system, or other logging backend.
 * 
 */
class Logger {
public:
    virtual ~Logger() = default;

    /**
     * @brief Log command received event
     */
    virtual void log_command_received(const Command& cmd) = 0;

    /**
     * @brief Log validation result
     */
    virtual void log_validation(const Command& cmd, const ValidationMetadata& validation) = 0;

    /**
     * @brief Log priority selection
     */
    virtual void log_priority_selection(CommandSource selected_source, const Command& cmd) = 0;

    /**
     * @brief Log command forwarded to BCM
     */
    virtual void log_command_forwarded(const Command& cmd) = 0;

    /**
     * @brief Log watchdog heartbeat
     */
    virtual void log_watchdog_heartbeat() = 0;

    /**
     * @brief Log system state transition
     */
    virtual void log_state_transition(SystemState from, SystemState to, const std::string& reason) = 0;

    /**
     * @brief Log error
     */
    virtual void log_error(const std::string& component, const std::string& message) = 0;

    /**
     * @brief Log general info message
     */
    virtual void log_info(const std::string& component, const std::string& message) = 0;
};

/**
 * @brief Console Logger Implementation
 * 
 * Simple implementation that logs to stdout/stderr.
 * Need to adapt to Nick's database logging system later.
 */
class ConsoleLogger : public Logger {
public:
    ConsoleLogger();
    ~ConsoleLogger() override = default;

    void log_command_received(const Command& cmd) override;
    void log_validation(const Command& cmd, const ValidationMetadata& validation) override;
    void log_priority_selection(CommandSource selected_source, const Command& cmd) override;
    void log_command_forwarded(const Command& cmd) override;
    void log_watchdog_heartbeat() override;
    void log_state_transition(SystemState from, SystemState to, const std::string& reason) override;
    void log_error(const std::string& component, const std::string& message) override;
    void log_info(const std::string& component, const std::string& message) override;

private:
    std::string get_timestamp_string() const;
    bool verbose_;
};

} // namespace command_processor

#endif // HEADER_FILES_LOGGER_H
