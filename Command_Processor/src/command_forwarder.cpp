#include "../header_files/command_forwarder.h"
#include <chrono>

namespace command_processor
{

    CommandForwarder::CommandForwarder(std::shared_ptr<PrioritySelector> priority_selector)
        : priority_selector_(priority_selector), config_{}, stats_{}
    {
        stats_.commands_forwarded = 0;
        stats_.no_command_cycles = 0;
        stats_.avg_loop_time_us = 0;
        stats_.max_loop_time_us = 0;
        stats_.last_forward_time = std::chrono::steady_clock::now();
    }

    CommandForwarder::~CommandForwarder()
    {
        stop();
    }

    void CommandForwarder::start(ForwardCallback callback)
    {
        if (running_.load())
        {
            return; // Already running
        }

        forward_callback_ = callback;
        running_.store(true);

        forwarding_thread_ = std::make_unique<std::thread>(&CommandForwarder::forwarding_loop, this);
    }

    void CommandForwarder::stop()
    {
        if (!running_.load())
        {
            return;
        }

        running_.store(false);

        if (forwarding_thread_ && forwarding_thread_->joinable())
        {
            forwarding_thread_->join();
        }
    }

    bool CommandForwarder::is_running() const
    {
        return running_.load();
    }

    CommandForwarder::Statistics CommandForwarder::get_statistics() const
    {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        return stats_;
    }

    void CommandForwarder::set_config(const Config &config)
    {
        config_ = config;
    }

    void CommandForwarder::forwarding_loop()
    {
        using steady_clock = std::chrono::steady_clock;
        auto period = config_.forward_period;

        auto next_wake = steady_clock::now() + period;

        while (running_.load())
        {
            auto loop_start = steady_clock::now();

            // Select highest priority command
            auto selected = priority_selector_->select();

            if (selected.has_value())
            {
                // Forward command to BCM
                if (forward_callback_)
                {
                    forward_callback_(selected.value());
                }

                std::lock_guard<std::mutex> lock(stats_mutex_);
                stats_.commands_forwarded++;
                stats_.last_forward_time = steady_clock::now();
            }
            else
            {
                // No valid command available
                std::lock_guard<std::mutex> lock(stats_mutex_);
                stats_.no_command_cycles++;
            }

            auto loop_end = steady_clock::now();
            auto loop_time_us = std::chrono::duration_cast<std::chrono::microseconds>(loop_end - loop_start).count();
            update_timing_stats(static_cast<uint32_t>(loop_time_us));

            // Sleep until next period
            std::this_thread::sleep_until(next_wake);
            next_wake += period;
        }
    }

    void CommandForwarder::update_timing_stats(uint32_t loop_time_us)
    {
        std::lock_guard<std::mutex> lock(stats_mutex_);

        // Update max
        if (loop_time_us > stats_.max_loop_time_us)
        {
            stats_.max_loop_time_us = loop_time_us;
        }

        // Update running average (simple moving average)
        if (stats_.commands_forwarded == 0)
        {
            stats_.avg_loop_time_us = loop_time_us;
        }
        else
        {
            const float alpha = 0.1f; // Smoothing factor
            stats_.avg_loop_time_us = static_cast<uint32_t>(
                alpha * loop_time_us + (1.0f - alpha) * stats_.avg_loop_time_us);
        }
    }

} // namespace command_processor
