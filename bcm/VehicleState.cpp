#include "VehicleState.hpp"
#include "StateStructures.hpp"
#include "UpdateSender.hpp"
#include <iostream>
#include <memory>
#include <optional>

VehicleState::VehicleState() {
    m_updater = std::make_shared<UpdateSender>();
}

bool VehicleState::setSpeed(const SpeedState& newSpeed) {
    m_speed = newSpeed;
    std::cout << "Vehicle Speed State updated to: " << newSpeed.speed << std::endl;
    m_updater->updateSpeed(m_speed.value());
    return true;
}

std::optional<SpeedState> VehicleState::getSpeed() {
    return m_speed;
}