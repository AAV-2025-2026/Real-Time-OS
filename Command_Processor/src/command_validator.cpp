#include "../header_files/command_validator.h"
#include <cmath>

namespace command_processor {

CommandValidator::CommandValidator() {
    // Initialize with default config
    config_ = Config{};
}

ValidationMetadata CommandValidator::validate(const Command& cmd) {
    ValidationMetadata metadata;
    metadata.validation_time = std::chrono::steady_clock::now();
    metadata.result = ValidationResult::VALID;
    metadata.reason = "Valid";

    // Check timestamp freshness
    if (!is_timestamp_fresh(cmd.timestamp)) {
        metadata.result = ValidationResult::STALE_TIMESTAMP;
        metadata.reason = "Command timestamp is stale";
        return metadata;
    }

    // Check sequence number validity
    if (!is_sequence_valid(cmd.source, cmd.sequence_number)) {
        metadata.result = ValidationResult::INVALID_SEQUENCE;
        metadata.reason = "Sequence number invalid or out of order";
        return metadata;
    }

    // Check sensor data ranges
    if (!is_sensor_data_valid(cmd.sensor_data)) {
        metadata.result = ValidationResult::OUT_OF_RANGE;
        metadata.reason = "Sensor data out of acceptable range";
        return metadata;
    }

    return metadata;
}

void CommandValidator::set_config(const Config& config) {
    config_ = config;
}

CommandValidator::Config CommandValidator::get_config() const {
    return config_;
}

void CommandValidator::reset() {
    last_sequence_numbers_.clear();
}

bool CommandValidator::is_timestamp_fresh(const std::chrono::steady_clock::time_point& timestamp) const {
    auto now = std::chrono::steady_clock::now();
    auto age_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp).count();
    
    return age_ms <= config_.freshness_timeout_ms.count();
}

bool CommandValidator::is_sequence_valid(CommandSource source, uint64_t sequence_number) {
    auto it = last_sequence_numbers_.find(source);
    
    if (it == last_sequence_numbers_.end()) {
        // First command from this source
        last_sequence_numbers_[source] = sequence_number;
        return true;
    }
    
    // Sequence number should be strictly increasing
    if (sequence_number <= it->second) {
        return false;  // Duplicate or replay
    }
    
    // Update last sequence number
    it->second = sequence_number;
    return true;
}

bool CommandValidator::is_sensor_data_valid(const Command::SensorData& data) const {
    // Check steering angle
    if (std::abs(data.steering_angle) > config_.max_steering_angle) {
        return false;
    }
    
    // Check speed
    if (data.speed < 0.0f || data.speed > config_.max_speed) {
        return false;
    }
    
    // Check acceleration
    if (std::abs(data.acceleration) > config_.max_acceleration) {
        return false;
    }
    
    // All checks passed
    return true;
}

} // namespace command_processor
