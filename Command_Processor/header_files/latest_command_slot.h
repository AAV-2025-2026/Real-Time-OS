#ifndef HEADER_FILES_LATEST_COMMAND_SLOT_H
#define HEADER_FILES_LATEST_COMMAND_SLOT_H

#include "types.h"
#include <mutex>
#include <optional>
#include <array>

namespace command_processor {

/**
 * @brief Thread-safe storage for the latest valid command per source
 * 
 * Stores the most recent valid command from each command source.
 * Prevents stale/outdated commands from being reused.
 * Thread-safe for concurrent access from intake and priority selector.
 */
class LatestCommandSlot {
public:
    LatestCommandSlot();
    ~LatestCommandSlot() = default;

    /**
     * @brief Store the latest command for a source (atomically)
     * @param cmd Command to store
     */
    void store(const Command& cmd);

    /**
     * @brief Retrieve the latest command for a source
     * @param source Source to retrieve from
     * @return Command if available and still fresh, empty optional otherwise
     */
    std::optional<Command> get(CommandSource source) const;

    /**
     * @brief Check if a command exists and is still fresh for a source
     * @param source Source to check
     * @return true if valid command exists
     */
    bool has_valid_command(CommandSource source) const;

    /**
     * @brief Clear command for a specific source
     * @param source Source to clear
     */
    void clear(CommandSource source);

    /**
     * @brief Clear all commands (e.g., during emergency stop)
     */
    void clear_all();

    /**
     * @brief Get age of command in a slot
     * @param source Source to check
     * @return Age in milliseconds, or max value if no command
     */
    uint32_t get_command_age_ms(CommandSource source) const;

private:
    /**
     * @brief Check if stored command is still fresh
     */
    bool is_fresh(const Command& cmd) const;

    // Use array indexed by CommandSource for O(1) access
    // Safety=0, Manual=1, Remote=2, Autonomous=3
    static constexpr size_t NUM_SOURCES = 4;
    
    struct Slot {
        std::optional<Command> command;
        mutable std::mutex mutex;
    };
    
    std::array<Slot, NUM_SOURCES> slots_;
};

} // namespace command_processor

#endif // HEADER_FILES_LATEST_COMMAND_SLOT_H
