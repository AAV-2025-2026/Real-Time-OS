#include "../header_files/priority_selector.h"

namespace command_processor {

PrioritySelector::PrioritySelector(std::shared_ptr<LatestCommandSlot> command_slot)
    : command_slot_(command_slot)
    , active_source_(CommandSource::NONE)
    , stats_{} {
    stats_.selections_made = 0;
    stats_.safety_selections = 0;
    stats_.manual_selections = 0;
    stats_.remote_selections = 0;
    stats_.autonomous_selections = 0;
    stats_.no_valid_command_count = 0;
    stats_.last_selected_source = CommandSource::NONE;
}

std::optional<Command> PrioritySelector::select() {
    stats_.selections_made++;
    
    auto selected = select_by_priority();
    
    if (!selected.has_value()) {
        stats_.no_valid_command_count++;
        active_source_ = CommandSource::NONE;
        stats_.last_selected_source = CommandSource::NONE;
        return std::nullopt;
    }
    
    // Update statistics based on selected source
    active_source_ = selected->source;
    stats_.last_selected_source = selected->source;
    
    switch (selected->source) {
        case CommandSource::SAFETY:
            stats_.safety_selections++;
            break;
        case CommandSource::MANUAL:
            stats_.manual_selections++;
            break;
        case CommandSource::REMOTE:
            stats_.remote_selections++;
            break;
        case CommandSource::AUTONOMOUS:
            stats_.autonomous_selections++;
            break;
        default:
            break;
    }
    
    return selected;
}

CommandSource PrioritySelector::get_active_source() const {
    return active_source_;
}

PrioritySelector::Statistics PrioritySelector::get_statistics() const {
    return stats_;
}

void PrioritySelector::reset_statistics() {
    stats_ = Statistics{};
    stats_.last_selected_source = CommandSource::NONE;
}

std::optional<Command> PrioritySelector::select_by_priority() {
    // Check sources in strict priority order
    
    // Priority 1: Safety (highest)
    auto cmd = command_slot_->get(CommandSource::SAFETY);
    if (cmd.has_value()) {
        return cmd;
    }
    
    // Priority 2: Manual
    cmd = command_slot_->get(CommandSource::MANUAL);
    if (cmd.has_value()) {
        return cmd;
    }
    
    // Priority 3: Remote
    cmd = command_slot_->get(CommandSource::REMOTE);
    if (cmd.has_value()) {
        return cmd;
    }
    
    // Priority 4: Autonomous (lowest)
    cmd = command_slot_->get(CommandSource::AUTONOMOUS);
    if (cmd.has_value()) {
        return cmd;
    }
    
    // No valid command from any source
    return std::nullopt;
}

} // namespace command_processor
