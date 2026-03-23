#include "VehicleState.hpp"
#include "StateStructures.hpp"
#include "UDPClient.hpp"
#include "UpdateSender.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <mutex>

VehicleState::VehicleState(std::shared_ptr<UDPClient> updatePublisher) {
    m_updater = std::make_shared<UpdateSender>(updatePublisher);
}

bool VehicleState::setIMU(const IMUState& newIMU) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_imu = newIMU;
    std::cout << "Vehicle IMU State updated to: " << newIMU.ang_vel_x << ", " << newIMU.ang_vel_y << ", " << newIMU.ang_vel_z << ". " << newIMU.lin_acc_x << ", " << newIMU.lin_acc_y << ", " << newIMU.lin_acc_z << std::endl;

    bool imuUpdateSuccess = false;
    for (uint8_t i = 0; i < 5; i++) {
        imuUpdateSuccess = m_updater->updateIMU(m_imu->getData());
        if (imuUpdateSuccess) {
            break;
        }
        std::cerr << "Failed to update IMU information, retrying" << std::endl;
    }
    if (!imuUpdateSuccess) {
        std::cerr << "Failed to update imu information too many times, aborting" << std::endl;
    }

    return imuUpdateSuccess;
}

bool VehicleState::setSpeed(const SpeedState& newSpeed) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_speed = newSpeed;
    std::cout << "Vehicle Speed State updated to: " << newSpeed.speed << std::endl;
    bool speedUpdateSuccess = false;
    for (uint8_t i = 0; i < 5; i++) { // 5 retry attempts
        speedUpdateSuccess = m_updater->updateSpeed(m_speed->getData());
        if (speedUpdateSuccess) {
            std::cout << "Successfully updated speed" << std::endl;
            break;
        }
        std::cerr << "Failed to update the speed information, retrying" << std::endl;
    }
    if (!speedUpdateSuccess) {
        std::cerr << "Failed to update speed information too many times, aborting" << std::endl;
    }

    Gear newGear{Gear::Park};
    if (newSpeed.speed > 0) {
        newGear = Gear::Drive;
    } else if (newSpeed.speed < 0) {
        newGear = Gear::Reverse;
    }
    m_gear = newGear;
    std::cout << "Vehicle Gear State updated to: " << newGear << std::endl;
    bool gearUpdateSuccess = false;
    for (uint8_t i = 0; i < 5; i++) { // 5 retry attemps
        gearUpdateSuccess = m_updater->updateGear(m_gear->getData());
        if (speedUpdateSuccess) {
            break;
        }
        std::cerr << "Failed to update the gear information, retrying" << std::endl;
    }
    if (!gearUpdateSuccess) {
        std::cerr << "Failed to update gear information too many times, aborting" << std::endl;
    }

    return speedUpdateSuccess && gearUpdateSuccess;
}

bool VehicleState::setDirection(const DirectionState& newDirection) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_direction = newDirection;
    std::cout << "Vehicle Direction State updated to: " << newDirection.direction << std::endl;
    bool updateDirectionSuccess = false;
    for (uint8_t i = 0; i < 5; i++) {
        updateDirectionSuccess = m_updater->updateDirection(m_direction->getData());
        if (updateDirectionSuccess) {
            break;
        }
        std::cerr << "Failed to update the direction information, retrying" << std::endl;
    }
    if (!updateDirectionSuccess) {
        std::cerr << "Failed to update direction information too many times, aborting" << std::endl;
    }

    return updateDirectionSuccess;
}

bool VehicleState::setLocation(const LocationState& newLocation) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_location = newLocation;
    std::cout << "Vehicle Location State updated to: " << "x: " << newLocation.x << ", y: " << newLocation.y << std::endl;
    bool updateLocationSuccess = false;
    for (uint8_t i = 0; i < 5; i++) {
        updateLocationSuccess = m_updater->updateLocation(m_location->getData());
        if (updateLocationSuccess) {
            break;
        }
        std::cerr << "Failed to update location information, retrying" << std::endl;
    }
    if (!updateLocationSuccess) {
        std::cerr << "Failed to update location information too many times, aborting" << std::endl;
    }
    return updateLocationSuccess;
}

void VehicleState::printState() {
    std::cout << "The current state is: ";

    std::cout << "Speed: ";
    if (m_speed.has_value()) {
        std::cout << m_speed->getData().speed;
    } else {
        std::cout << "unintitalized speed";
    }
    std::cout << ", ";

    std::cout << "Direction: ";
    if (m_direction.has_value()) {
        std::cout << m_direction->getData().direction;
    } else {
        std::cout << "uninitialized direction";
    }
    std::cout << ", ";

    std::cout << "Location: ";
    if (m_location.has_value()) {
        std::cout << "x: " << m_location->getData().x << ", y: " << m_location->getData().y;
    } else {
        std::cout << "uninitialized location";
    }
    std::cout << ", ";

    std::cout << "Gear: ";
    if (m_gear.has_value()) {
        switch (m_gear->getData()) {
            case Gear::Park:
                std::cout << "Park";
                break;
            case Gear::Drive:
                std::cout << "Drive";
                break;
            case Gear::Reverse:
                std::cout << "Reverse";
                break;
            default:
                std::cout << "invalid gear";
                break;
        }
    } else {
        std::cout << "uninitialized gear";
    }

    std::cout << std::endl;
}
