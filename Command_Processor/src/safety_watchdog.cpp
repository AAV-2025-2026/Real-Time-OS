#include "../header_files/safety_watchdog.h"
#include <chrono>

namespace command_processor {

SafetyWatchdog::SafetyWatchdog()
    : config_{}
    , stats_{} {
    stats_.heartbeats_sent = 0;
    stats_.feeds_received = 0;
    stats_.time_since_last_feed_ms = 0;
    
    last_feed_time_ = std::chrono::steady_clock::now();
}

SafetyWatchdog::~SafetyWatchdog() {
    stop();
}

void SafetyWatchdog::start(std::function<void()> heartbeat_callback) {
    if (running_.load()) {
        return;  // Already running
    }
    
    heartbeat_callback_ = heartbeat_callback;
    last_feed_time_ = std::chrono::steady_clock::now();
    
    running_.store(true);
    heartbeat_thread_ = std::make_unique<std::thread>(&SafetyWatchdog::heartbeat_loop, this);
}

void SafetyWatchdog::stop() {
    if (!running_.load()) {
        return;
    }
    
    running_.store(false);
    
    if (heartbeat_thread_ && heartbeat_thread_->joinable()) {
        heartbeat_thread_->join();
    }
}

void SafetyWatchdog::feed() {
    std::lock_guard<std::mutex> lock(feed_mutex_);
    last_feed_time_ = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> stats_lock(stats_mutex_);
    stats_.feeds_received++;
}

bool SafetyWatchdog::is_running() const {
    return running_.load();
}

void SafetyWatchdog::set_config(const Config& config) {
    config_ = config;
}

SafetyWatchdog::Statistics SafetyWatchdog::get_statistics() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    return stats_;
}

void SafetyWatchdog::heartbeat_loop() {
    using namespace std::chrono;
    
    std::chrono::milliseconds heartbeat_period = config_.heartbeat_period_ms;
    auto next_heartbeat = steady_clock::now() + heartbeat_period;
    
    while (running_.load()) {
        // Send heartbeat to external watchdog component
        if (heartbeat_callback_) {
            heartbeat_callback_();
            
            std::lock_guard<std::mutex> lock(stats_mutex_);
            stats_.heartbeats_sent++;
        }
        
        // Update time since last feed (for monitoring/debugging)
        {
            std::lock_guard<std::mutex> feed_lock(feed_mutex_);
            auto now = steady_clock::now();
            auto time_since_feed = duration_cast<milliseconds>(now - last_feed_time_);
            
            std::lock_guard<std::mutex> stats_lock(stats_mutex_);
            stats_.time_since_last_feed_ms = static_cast<uint32_t>(time_since_feed.count());
        }
        
        // Sleep until next heartbeat
        std::this_thread::sleep_until(next_heartbeat);
        next_heartbeat += heartbeat_period;
    }
}

} // namespace command_processor
