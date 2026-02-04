#ifndef HEADER_FILES_COMMAND_VALIDATOR_H
#define HEADER_FILES_COMMAND_VALIDATOR_H

#include "types.h"
#include <unordered_map>

namespace command_processor {

/**
 * @brief Command Validator
 * 
 * Validates commands for:
 * - Structure correctness
 * - Timestamp freshness (prevents stale data)
 * - Sequence order (prevents replay attacks)
 * - Range checking on sensor data
 */
class CommandValidator {
public:
    CommandValidator();
    ~CommandValidator() = default;

    /**
     * @brief Validate a command
     * @param cmd Command to validate
     * @return Validation result with metadata
     */
    ValidationMetadata validate(const Command& cmd);

    /**
     * @brief Configure validation parameters
     */
    struct Config {
        std::chrono::milliseconds freshness_timeout_ms = COMMAND_FRESHNESS_TIMEOUT_MS;
        
        // Sensor data range limits (example values, idk what sensor data im even gonna get)
        float max_steering_angle = 45.0f;     // degrees
        float max_speed = 30.0f;              // m/s (~108 km/h)
        float max_acceleration = 5.0f;        // m/s^2
    };
    
    void set_config(const Config& config);
    Config get_config() const;

    /**
     * @brief Reset validator state (e.g., after fault recovery)
     */
    void reset();

private:
    /**
     * @brief Check if timestamp is fresh
     */
    bool is_timestamp_fresh(const std::chrono::steady_clock::time_point& timestamp) const;

    /**
     * @brief Check if sequence number is valid (monotonically increasing per source)
     */
    bool is_sequence_valid(CommandSource source, uint64_t sequence_number);

    /**
     * @brief Validate sensor data ranges (needs to be adapted when real data structure changes)
     */
    bool is_sensor_data_valid(const Command::SensorData& data) const;

    Config config_;
    
    // Track last sequence number per source to detect replay/duplication
    std::unordered_map<CommandSource, uint64_t> last_sequence_numbers_;
};

} // namespace command_processor

#endif // HEADER_FILES_COMMAND_VALIDATOR_H
