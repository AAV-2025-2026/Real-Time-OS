#include "../header_files/latest_command_slot.h"

namespace command_processor {

LatestCommandSlot::LatestCommandSlot() {
    // Initialize all slots as empty
    for (auto& slot : slots_) {
        slot.command = std::nullopt;
    }
}

void LatestCommandSlot::store(const Command& cmd) {
    size_t index = static_cast<size_t>(cmd.source);
    
    if (index >= NUM_SOURCES) {
        return;  // Invalid source
    }
    
    std::lock_guard<std::mutex> lock(slots_[index].mutex);
    slots_[index].command = cmd;
}

std::optional<Command> LatestCommandSlot::get(CommandSource source) const {
    size_t index = static_cast<size_t>(source);
    
    if (index >= NUM_SOURCES) {
        return std::nullopt;
    }
    
    std::lock_guard<std::mutex> lock(slots_[index].mutex);
    
    if (!slots_[index].command.has_value()) {
        return std::nullopt;
    }
    
    // Check if command is still fresh
    if (!is_fresh(slots_[index].command.value())) {
        return std::nullopt;
    }
    
    return slots_[index].command;
}

bool LatestCommandSlot::has_valid_command(CommandSource source) const {
    return get(source).has_value();
}

void LatestCommandSlot::clear(CommandSource source) {
    size_t index = static_cast<size_t>(source);
    
    if (index >= NUM_SOURCES) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(slots_[index].mutex);
    slots_[index].command = std::nullopt;
}

void LatestCommandSlot::clear_all() {
    for (size_t i = 0; i < NUM_SOURCES; ++i) {
        std::lock_guard<std::mutex> lock(slots_[i].mutex);
        slots_[i].command = std::nullopt;
    }
}

uint32_t LatestCommandSlot::get_command_age_ms(CommandSource source) const {
    size_t index = static_cast<size_t>(source);
    
    if (index >= NUM_SOURCES) {
        return UINT32_MAX;
    }
    
    std::lock_guard<std::mutex> lock(slots_[index].mutex);
    
    if (!slots_[index].command.has_value()) {
        return UINT32_MAX;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - slots_[index].command->timestamp);
    
    return static_cast<uint32_t>(age.count());
}

bool LatestCommandSlot::is_fresh(const Command& cmd) const {
    auto now = std::chrono::steady_clock::now();
    auto age_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - cmd.timestamp).count();
    
    return age_ms <= COMMAND_FRESHNESS_TIMEOUT_MS.count();
}

} // namespace command_processor
