#ifndef HEADER_FILES_COMMAND_INTAKE_H
#define HEADER_FILES_COMMAND_INTAKE_H

#include "types.h"
#include <memory>
#include <functional>

namespace command_processor {

/**
 * @brief Command Intake Interface
 * 
 * Responsible for receiving commands (ROS2),
 * normalizing them into internal format, and attaching metadata.
 * 
 */
class CommandIntake {
public:
    using CommandCallback = std::function<void(const Command&)>;

    CommandIntake() = default;
    virtual ~CommandIntake() = default;

    /**
     * @brief Register callback for when commands are received
     * @param callback Function to call with normalized command
     */
    void set_command_callback(CommandCallback callback);

    /**
     * @brief Process incoming raw command from external source
     * @param source Command source identifier
     * @param raw_data Raw sensor data (needs to be adapted when ROS2 format is known)
     * @param sequence_number Sequence number from source
     */
    void receive_command(CommandSource source, 
                        const Command::SensorData& raw_data,
                        uint64_t sequence_number);

    /**
     * @brief Get statistics for monitoring
     */
    struct Statistics {
        uint64_t commands_received;
        uint64_t commands_normalized;
        std::chrono::steady_clock::time_point last_received_time;
    };
    
    Statistics get_statistics() const;

private:
    /**
     * @brief Normalize raw command into internal format
     */
    Command normalize_command(CommandSource source,
                              const Command::SensorData& raw_data,
                              uint64_t sequence_number);

    CommandCallback command_callback_;
    Statistics stats_{0, 0, std::chrono::steady_clock::now()};
};

} // namespace command_processor

#endif // HEADER_FILES_COMMAND_INTAKE_H
