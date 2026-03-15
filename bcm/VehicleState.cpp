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
    StateObject<SpeedState> newSpeedObject(newSpeed);
    m_speed = newSpeedObject;
    std::cout << "Vehicle Speed State updated to: " << newSpeed.speed << std::endl;
    m_updater->updateSpeed(m_speed.value().getData());
    return true;
}

std::optional<SpeedState> VehicleState::getSpeed() {
    if (m_speed) {
        return m_speed.value().getData();
    } else {
        return {};
    }
}

void VehicleState::printState() {
    std::cout << "The current state is: ";

    std::cout << "Speed: " << m_speed->getData().speed << ", ";

    std::cout << std::endl;
}