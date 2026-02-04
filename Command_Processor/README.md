# Command Processing Module

A safety-critical, real-time command processing system for autonomous vehicle control on QNX RTOS.

## Overview

The Command Processing module acts as the bridge between high-level decision-making (manual control, remote operation, or autonomy) and low-level actuation through the vehicle's Body Control Module (BCM). It implements deterministic, priority-based command arbitration with comprehensive safety mechanisms.

## Architecture

The system implements a pipeline architecture with the following components:

```
Command Sources → Command Intake → Validator → Latest Slot → Priority Selector → Forwarder → BCM
                                                                      ↓
                                                          Heartbeat Generator
                                                                      ↓
                                                    External Watchdog Component
                                                    (handles timeout & emergency stop)
```

### Components

1. **Command Intake** (`command_intake.h/cpp`)
   - Receives commands from external sources (ROS2)
   - Normalizes into internal format
   - Attaches metadata (timestamps, source identifiers)

2. **Command Validator** (`command_validator.h/cpp`)
   - Validates structure, timestamp freshness, sequence order
   - Prevents replay attacks and stale data
   - Range-checks sensor data

3. **Latest Command Slot** (`latest_command_slot.h/cpp`)
   - Thread-safe storage of most recent valid command per source
   - Automatic staleness detection
   - O(1) access by command source

4. **Priority Selector** (`priority_selector.h/cpp`)
   - Deterministic priority-based selection
   - Priority order: SAFETY > MANUAL > REMOTE > AUTONOMOUS
   - Single active command at any time

5. **Command Forwarder** (`command_forwarder.h/cpp`)
   - Real-time task running at 10ms period
   - Forwards selected commands to BCM
   - Maintains timing statistics

6. **Safety Watchdog** (`safety_watchdog.h/cpp`)
   - Sends periodic heartbeats to external system watchdog component
   - Proves Command Processor is alive
   - External watchdog handles all timeout detection and emergency stop logic

7. **Logger** (`logger.h/cpp`)
   - Abstract logging interface
   - Console implementation included
   - Easy to replace with RTOS database logger

## Design Principles

- **Safety First**: Safety commands always take precedence; system fails safe
- **Determinism**: Same inputs always produce same behavior with bounded execution time
- **Priority-Based Arbitration**: Explicit selection, no command blending
- **Modularity**: Components have single responsibilities and clean interfaces
- **Real-Time Performance**: 10ms forwarding period, <100ms end-to-end latency

## Building

### Prerequisites

- CMake 3.10+
- C++17 compiler

### Build Instructions

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run example
.\Debug\main_example.exe
```

### QNX Cross-Compilation

~To be done

## Usage

### Basic Usage

```cpp
#include "header_files/command_processor.h"

// Create processor
auto processor = std::make_unique<CommandProcessor>();

// Initialize with callbacks
processor->initialize(
    // BCM callback
    [](const Command& cmd) {
        // Send to Body Control Module
    },
    // Watchdog heartbeat callback
    []() {
        // Pulse external watchdog component to prove we're alive
        // The external watchdog handles all timeout and emergency stop logic
    }
);

// Start processing
processor->start();

// Process incoming commands (e.g., from ROS2)
Command::SensorData data;
data.steering_angle = 10.0f;
data.speed = 5.0f;
processor->process_command(CommandSource::REMOTE, data, sequence_num);

// Stop when done
processor->stop();
```

### Configuration

```cpp
CommandProcessor::Config config;

// Validator configuration
config.validator_config.freshness_timeout_ms = 200;

// Example values to change when real processed data format is decided
config.validator_config.max_steering_angle = 45.0f;
config.validator_config.max_speed = 30.0f;

// Forwarder configuration
config.forwarder_config.forward_period_ms = 10;
config.forwarder_config.send_heartbeat_on_no_command = true;

// Watchdog configuration
config.watchdog_config.heartbeat_period_ms = 50;
config.watchdog_config.timeout_ms = 200;

processor->set_config(config);
```

### Integration with ROS2

~To be done

## Testing

The included example program (`main_example.cpp`) demonstrates:
- Command reception from multiple sources
- Priority-based selection
- Validation and rejection of invalid commands
- Statistics monitoring
- System lifecycle management

Run it to verify the system behavior:



## Timing Requirements

| Parameter | Value | Notes |
|-----------|-------|-------|
| Forward Period | 10 ms | Command forwarding rate |
| Watchdog Timeout | 200 ms | Max time without feed |
| Command Freshness | 200 ms | Max command age |
| Target Latency | <100 ms | End-to-end goal |

## Safety Features

1. **Command Freshness**: Prevents stale data from being applied
2. **Sequence Validation**: Prevents replay attacks
3. **Range Checking**: Validates sensor data bounds
4. **Priority Enforcement**: Safety commands always win
5. **Watchdog Supervision**: Independent timeout detection
6. **Fail-Safe Design**: System defaults to safe state on failure

## File Structure

```
command_processor/
├── CMakeLists.txt
├── README.md
├── header_files/
│   ├── types.h                    # Common types and constants
│   ├── command_intake.h           # Command intake interface
│   ├── command_validator.h        # Validation logic
│   ├── latest_command_slot.h      # Command storage
│   ├── priority_selector.h        # Priority arbitration
│   ├── command_forwarder.h        # Real-time forwarder
│   ├── safety_watchdog.h          # Command Processor Heartbeat
│   ├── logger.h                   # Logging interface
│   └── command_processor.h        # Main orchestrator
└── src/
    ├── command_intake.cpp
    ├── command_validator.cpp
    ├── latest_command_slot.cpp
    ├── priority_selector.cpp
    ├── command_forwarder.cpp
    ├── safety_watchdog.cpp
    ├── logger.cpp
    ├── command_processor.cpp
    └── main_example.cpp               # Example program
```

## Next Steps

1. **Skeleton Implementation** (Complete)
2. **ROS2 Integration**: Create ROS2 node wrapper
3. **BCM Interface**: Implement actual BCM communication
4. **Hardware Watchdog**: Connect to QNX hardware watchdog
5. **Timing Validation**: Measure on Raspberry Pi hardware
6. **Unit Tests**: Add comprehensive test suite
7. **Integration Tests**: Test with actual vehicle hardware
