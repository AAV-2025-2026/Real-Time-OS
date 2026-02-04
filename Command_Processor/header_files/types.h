#ifndef HEADER_FILES_TYPES_H
#define HEADER_FILES_TYPES_H

#include <cstdint>
#include <chrono>
#include <string>

namespace command_processor
{
    using namespace std::chrono_literals;

    // Timing constants (in milliseconds)
    inline const std::chrono::milliseconds COMMAND_FORWARD_PERIOD_MS = std::chrono::milliseconds(10ms);
    inline const std::chrono::milliseconds WATCHDOG_TIMEOUT_MS = std::chrono::milliseconds(200ms);
    inline const std::chrono::milliseconds COMMAND_FRESHNESS_TIMEOUT_MS = std::chrono::milliseconds(200ms);
    inline const std::chrono::milliseconds MAX_LATENCY_TARGET_MS = std::chrono::milliseconds(100ms);

    // Command source priorities (lower number = higher priority)
    enum class CommandSource : uint8_t
    {
        SAFETY = 0,
        MANUAL = 1,
        REMOTE = 2,
        AUTONOMOUS = 3,
        NONE = 255
    };

    // Command validation result
    enum class ValidationResult : uint8_t
    {
        VALID,
        INVALID_STRUCTURE,
        STALE_TIMESTAMP,
        INVALID_SEQUENCE,
        OUT_OF_RANGE
    };

    // System state
    enum class SystemState : uint8_t
    {
        INITIALIZING,
        NORMAL_OPERATION,
        SAFE_MODE,
        EMERGENCY_STOP,
        FAULT
    };

    // Command structure (normalized internal format)
    struct Command
    {
        CommandSource source;
        uint64_t sequence_number;
        std::chrono::steady_clock::time_point timestamp;

        // Temp sensor data structure
        struct SensorData
        {
            float steering_angle; // degrees
            float speed;          // m/s
            float acceleration;   // m/s^2
            bool brake_engaged;
        } sensor_data;

        Command()
            : source(CommandSource::NONE), sequence_number(0), timestamp(std::chrono::steady_clock::now()), sensor_data{0.0f, 0.0f, 0.0f, false} {}
    };

    // Validation metadata
    struct ValidationMetadata
    {
        ValidationResult result;
        std::string reason;
        std::chrono::steady_clock::time_point validation_time;
    };

    // Logging event types
    enum class LogEventType : uint8_t
    {
        COMMAND_RECEIVED,
        COMMAND_VALIDATED,
        COMMAND_REJECTED,
        PRIORITY_SELECTED,
        COMMAND_FORWARDED,
        WATCHDOG_HEARTBEAT,
        STATE_TRANSITION,
        ERROR
    };

    // Helper function to convert CommandSource to string
    inline const char *command_source_to_string(CommandSource source)
    {
        switch (source)
        {
        case CommandSource::SAFETY:
            return "SAFETY";
        case CommandSource::MANUAL:
            return "MANUAL";
        case CommandSource::REMOTE:
            return "REMOTE";
        case CommandSource::AUTONOMOUS:
            return "AUTONOMOUS";
        case CommandSource::NONE:
            return "NONE";
        default:
            return "UNKNOWN";
        }
    }

    // Helper function to convert ValidationResult to string
    inline const char *validation_result_to_string(ValidationResult result)
    {
        switch (result)
        {
        case ValidationResult::VALID:
            return "VALID";
        case ValidationResult::INVALID_STRUCTURE:
            return "INVALID_STRUCTURE";
        case ValidationResult::STALE_TIMESTAMP:
            return "STALE_TIMESTAMP";
        case ValidationResult::INVALID_SEQUENCE:
            return "INVALID_SEQUENCE";
        case ValidationResult::OUT_OF_RANGE:
            return "OUT_OF_RANGE";
        default:
            return "UNKNOWN";
        }
    }

} // namespace command_processor

#endif // HEADER_FILES_TYPES_H
