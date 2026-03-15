#ifndef VEHICLE_STATE_HPP
#define VEHICLE_STATE_HPP

#include <optional>
#include "StateStructures.hpp"
#include "UpdateSender.hpp"
#include <memory>

class VehicleState {
public:
    VehicleState();
    bool setSpeed(const SpeedState& newSpeed);
    std::optional<SpeedState> getSpeed();

    void printState();

private:
    std::shared_ptr<UpdateSender> m_updater;
    std::optional<StateObject<SpeedState>> m_speed;
};

#endif