#ifndef HEADER_FILES_PRIORITY_SELECTOR_H
#define HEADER_FILES_PRIORITY_SELECTOR_H

#include "types.h"
#include "latest_command_slot.h"
#include <memory>
#include <optional>

namespace command_processor {

/**
 * @brief Priority-based Command Selector
 * 
 * Evaluates all command sources and selects the highest-priority
 * command that is still valid.
 * 
 * Priority Order (highest to lowest):
 * 1. SAFETY (always takes precedence)
 * 2. MANUAL
 * 3. REMOTE
 * 4. AUTONOMOUS
 * 
 */
class PrioritySelector {
public:
    explicit PrioritySelector(std::shared_ptr<LatestCommandSlot> command_slot);
    ~PrioritySelector() = default;

    /**
     * @brief Select the highest priority valid command
     * @return Selected command if any source has valid command, empty otherwise
     */
    std::optional<Command> select();

    /**
     * @brief Get the currently active command source
     * @return Active source or NONE if no valid command
     */
    CommandSource get_active_source() const;

    /**
     * @brief Selection statistics for monitoring
     */
    struct Statistics {
        uint64_t selections_made;
        uint64_t safety_selections;
        uint64_t manual_selections;
        uint64_t remote_selections;
        uint64_t autonomous_selections;
        uint64_t no_valid_command_count;
        CommandSource last_selected_source;
    };
    
    Statistics get_statistics() const;

    /**
     * @brief Reset statistics
     */
    void reset_statistics();

private:
    /**
     * @brief Check sources in priority order
     */
    std::optional<Command> select_by_priority();

    std::shared_ptr<LatestCommandSlot> command_slot_;
    CommandSource active_source_;
    Statistics stats_;
};

} // namespace command_processor

#endif // HEADER_FILES_PRIORITY_SELECTOR_H
