#include "../header_files/command_intake.h"

namespace command_processor {

void CommandIntake::set_command_callback(CommandCallback callback) {
    command_callback_ = callback;
}

void CommandIntake::receive_command(CommandSource source,
                                    const Command::SensorData& raw_data,
                                    uint64_t sequence_number) {
    stats_.commands_received++;
    stats_.last_received_time = std::chrono::steady_clock::now();

    // Normalize command into internal format
    Command normalized_cmd = normalize_command(source, raw_data, sequence_number);
    
    stats_.commands_normalized++;

    // Forward to callback (typically the main CommandProcessor)
    if (command_callback_) {
        command_callback_(normalized_cmd);
    }
}

Command CommandIntake::normalize_command(CommandSource source,
                                         const Command::SensorData& raw_data,
                                         uint64_t sequence_number) {
    Command cmd;
    cmd.source = source;
    cmd.sequence_number = sequence_number;
    cmd.timestamp = std::chrono::steady_clock::now();
    cmd.sensor_data = raw_data;
    
    return cmd;
}

CommandIntake::Statistics CommandIntake::get_statistics() const {
    return stats_;
}

} // namespace command_processor
