#ifndef MANUAL_INPUT_HPP
#define MANUAL_INPUT_HPP

#include <memory>
#include <thread>
#include "vehicle_state.hpp"
#include <atomic>

namespace vehicle_state {
class ManualInput {
public:
    ManualInput(std::shared_ptr<VehicleState> vehicleState);
    void readInput();
    void stop();

private:
    std::shared_ptr<VehicleState> m_vehicleState;
    std::thread m_readThread;
    std::atomic_bool m_running{true};
};
}

#endif